#!/usr/bin/env python3
"""Generate and verify notes/README.md from notes/data/notes-registry.tsv.

    python tools/notes_index.py            # print the generated README to stdout
    python tools/notes_index.py --write    # regenerate notes/README.md in place
    python tools/notes_index.py --check    # CI gate: fail if the tree disagrees with it

WHY THIS EXISTS
---------------
`notes/` grew to 136 files with zero entry point: no `notes/README.md`, and the two
front doors that exist (`AGENTS.md`, `CONTRIBUTING.md`) together cite 7 of them. An
agent reading the repo's own onboarding docs end-to-end learned of 7 notes out of 136
and found the rest only by already knowing the filename, or by grepping 2.7MB blind.

This is the fix, and it has to be a generated page, not a hand-maintained one, because
a hand-maintained index drifts the moment someone adds a file without updating it --
which is exactly how `notes/` got here (append-only, 76 of 113 files added in one
month, nothing ever indexed). `notes/data/notes-registry.tsv` is the single source of
truth (path, tier, bucket, status, owner_tool, one-line summary); this tool only
renders it and checks the tree agrees with it. It never edits a note's own content or
inserts a per-file metadata header -- see `notes/CONVENTIONS.md` for why that idea was
rejected (a fourth metadata idiom, and it would fail on every regeneration of a
generated note like `cpp-tu-current-state.md`).

TIER IS READ MODE, NOT DIRECTORY
---------------------------------
`doctrine` is what an agent loads whole to *do* work: craft rules, runbooks, the
role prompts under `notes/agents/roles/`, this policy file, this index. `evidence` is
what it looks up ONE fact in and never reads whole: provenance censuses, pilot
reports, the codegen discovery log. `data` is machine-read/written (`*.tsv`,
`*.json`, `*.jsonl`); `archive` is the frozen `notes/archive/` ledger of retired
notes. Classifying by directory was tried and rejected: `notes/agents/roles/writer.md`
(44.7KB) and `builder.md` (31.8KB) are role prompts an agent loads every time it does
the work, so they count as doctrine even though they sit in a directory that also
holds process/orchestration files -- the registry's `tier` column is the only
authority, never the path.

WHAT `--check` VERIFIES
------------------------
(a) every file `git` tracks or would track (i.e. not gitignored) under `notes/` has a
    registry row;
(b) every registry row names a file that actually exists;
(c) `notes/README.md` on disk is byte-identical to what this tool would generate;
(d) the `doctrine` tier's total bytes, reported against the 150KB cap named in
    `notes/CONVENTIONS.md`'s design discussion -- REPORTED, not gated: today's honest
    total is well over the cap (doctrine condensation is a separate, not-yet-run
    piece of work), and failing the whole gate on that would make `--check` red
    forever for a reason this tool cannot fix by itself.

Only (a), (b) and (c) affect the exit code. Read-only: no ROM, no compiler, seconds.
"""
from __future__ import annotations

import argparse
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
REGISTRY = REPO / "notes" / "data" / "notes-registry.tsv"
README = REPO / "notes" / "README.md"

FIELDS = ("path", "tier", "bucket", "status", "owner_tool", "summary")
TIERS = ("doctrine", "evidence", "agent", "data", "archive")
STATUSES = ("live", "superseded", "historical")
DOCTRINE_CAP_BYTES = 150 * 1024

# Bucket display order: roughly the order a newcomer's questions arrive in --
# "how do I work here" before "how do I name things" before the domain-evidence
# buckets, with build/setup reference last since it's consulted least often.
BUCKET_ORDER = (
    "process",
    "matching",
    "naming",
    "provenance",
    "cpp-conversion",
    "tu-promotion",
    "build",
)

TIER_LABEL = {
    "doctrine": "doctrine",
    "agent": "doctrine",  # same load-bearing/whole-load contract; see module docstring
    "evidence": "evidence",
    "data": "data",
    "archive": "archive (frozen)",
}


class RegistryError(RuntimeError):
    pass


def tracked_or_trackable_notes_files(repo: pathlib.Path) -> list[str]:
    """Every real ``notes/**`` file git would include in a commit right now.

    Union of tracked and untracked-but-not-gitignored, so a freshly-added file that
    is not yet staged still counts (mirrors ``cpp_tu_state.dirty_authority_inputs``'s
    tracked+untracked union). Gitignored scratch (session drafts, per-agent queue
    TSVs -- see ``.gitignore``) is correctly excluded either way: it should not need
    a registry row.
    """
    try:
        tracked = subprocess.run(
            ["git", "-C", str(repo), "ls-files", "-z", "--", "notes"],
            check=True, capture_output=True,
        ).stdout.decode("utf-8", errors="surrogateescape")
        untracked = subprocess.run(
            ["git", "-C", str(repo), "ls-files", "-z", "--others",
             "--exclude-standard", "--", "notes"],
            check=True, capture_output=True,
        ).stdout.decode("utf-8", errors="surrogateescape")
    except (OSError, subprocess.CalledProcessError) as exc:
        raise RegistryError(f"cannot list notes/ via git: {exc}") from exc
    paths = {p for p in (tracked + untracked).split("\0") if p}
    return sorted(paths)


def load_registry(path: pathlib.Path) -> list[dict[str, str]]:
    if not path.is_file():
        raise RegistryError(f"{path.relative_to(REPO)}: not found")
    text = path.read_text(encoding="utf-8")
    lines = text.split("\n")
    expected_header = "\t".join(FIELDS)
    if not lines or lines[0].split("\t") != list(FIELDS):
        raise RegistryError(
            f"{path.relative_to(REPO)}: header is {lines[0]!r}, "
            f"expected {expected_header!r}"
        )
    # Trailing blank line from the final newline is expected; anything else blank
    # in the middle is a hand-editing mistake worth catching.
    body = lines[1:]
    if body and body[-1] == "":
        body = body[:-1]
    rows = []
    for lineno, line in enumerate(body, start=2):
        if not line.strip():
            raise RegistryError(f"{path.relative_to(REPO)}:{lineno}: blank row")
        cols = line.split("\t")
        if len(cols) != len(FIELDS):
            raise RegistryError(
                f"{path.relative_to(REPO)}:{lineno}: {len(cols)} columns, "
                f"expected {len(FIELDS)}"
            )
        rows.append(dict(zip(FIELDS, cols)))
    return rows


def validate(rows: list[dict[str, str]], repo: pathlib.Path) -> list[str]:
    errors = []
    seen: dict[str, int] = {}
    for row in rows:
        path = row["path"]
        seen[path] = seen.get(path, 0) + 1
        if row["tier"] not in TIERS:
            errors.append(f"{path}: unknown tier {row['tier']!r}")
        if row["status"] not in STATUSES:
            errors.append(f"{path}: unknown status {row['status']!r}")
        if not row["bucket"]:
            errors.append(f"{path}: empty bucket")
        if not row["summary"]:
            errors.append(f"{path}: empty summary")
        if not path.startswith("notes/"):
            errors.append(f"{path}: registry path must start with notes/")
        elif not (repo / path).is_file():
            errors.append(f"{path}: registry row names a file that does not exist")
    for path, count in seen.items():
        if count > 1:
            errors.append(f"{path}: {count} registry rows for the same file")

    actual = set(tracked_or_trackable_notes_files(repo))
    registered = set(seen)
    for path in sorted(actual - registered):
        errors.append(f"{path}: under notes/ but has no registry row")
    for path in sorted(registered - actual):
        errors.append(f"{path}: registry row for a file git does not track under notes/")
    return errors


def doctrine_bytes(rows: list[dict[str, str]], repo: pathlib.Path) -> tuple[int, list[str]]:
    files = sorted(row["path"] for row in rows if row["tier"] in ("doctrine", "agent"))
    total = 0
    for path in files:
        full = repo / path
        if full.is_file():
            total += full.stat().st_size
    return total, files


def _bucket_of(row: dict[str, str]) -> str:
    return row["bucket"]


def render_readme(rows: list[dict[str, str]], repo: pathlib.Path) -> str:
    total_doctrine, doctrine_files = doctrine_bytes(rows, repo)
    tier_counts: dict[str, int] = {}
    for row in rows:
        tier_counts[row["tier"]] = tier_counts.get(row["tier"], 0) + 1

    buckets: dict[str, list[dict[str, str]]] = {}
    for row in rows:
        buckets.setdefault(_bucket_of(row), []).append(row)
    ordered_buckets = list(BUCKET_ORDER) + sorted(b for b in buckets if b not in BUCKET_ORDER)

    lines: list[str] = [
        "# notes/ index",
        "",
        "<!-- Generated by tools/notes_index.py --write. Do not hand-edit; edit",
        "     notes/data/notes-registry.tsv and regenerate. -->",
        "",
        "A router, not a summary: find the bucket that matches your question, then the",
        "file whose one-line summary answers it. Read `notes/CONVENTIONS.md` before",
        "adding, retiring, or moving anything here.",
        "",
        "**Doctrine** (bold below) is what you load whole to do the work -- craft rules,",
        "runbooks, role prompts. **Evidence** is what you look up one fact in and never",
        "read whole -- provenance, censuses, pilot reports. `data` is machine-read/",
        "written; `archive` is frozen history. See `notes/CONVENTIONS.md` §1.",
        "",
        f"Doctrine budget: **{total_doctrine:,} bytes ({total_doctrine / 1024:.1f}KB)** "
        f"across {len(doctrine_files)} files, against a stated 150KB cap.",
        "Over cap today -- reported here, not enforced (see `tools/notes_index.py`'s",
        "docstring). Condensing doctrine is separate, not-yet-run work.",
        "",
        "Tier counts: " + ", ".join(
            f"{tier_counts.get(t, 0)} {t}" for t in TIERS if tier_counts.get(t)
        ) + f" ({sum(tier_counts.values())} files total).",
        "",
    ]

    for bucket in ordered_buckets:
        bucket_rows = buckets[bucket]
        # doctrine/agent first (the load-bearing ones), then evidence, then data,
        # then archive -- within a tier group, alphabetical by path.
        tier_rank = {"doctrine": 0, "agent": 0, "evidence": 1, "data": 2, "archive": 3}
        bucket_rows = sorted(
            bucket_rows, key=lambda r: (tier_rank.get(r["tier"], 9), r["path"])
        )
        lines.append(f"## {bucket}")
        lines.append("")
        for row in bucket_rows:
            name = row["path"].removeprefix("notes/")
            summary = row["summary"]
            tags = []
            # A real link, not just a backtick-quoted filename: this is what makes every
            # notes/ file reachable in one hop from this page for a link-following reader
            # (human or agent) -- see tools/notes_bench.py, which models exactly that walk
            # and could not reach notes/agents/roles/writer.md before this changed.
            if row["tier"] in ("doctrine", "agent"):
                label = f"**[`{name}`]({name})**"
            else:
                label = f"[`{name}`]({name})"
            if row["status"] == "superseded":
                tags.append("superseded")
            elif row["status"] == "historical":
                tags.append("historical")
            if row["tier"] == "archive":
                tags.append("archive")
            elif row["tier"] == "data":
                tags.append("data")
            tag_text = f" _{', '.join(tags)}_" if tags else ""
            lines.append(f"- {label}{tag_text} — {summary}")
        lines.append("")

    lines.append(
        "Regenerate with `python tools/notes_index.py --write`; "
        "`python tools/notes_index.py --check` verifies this page, the registry, "
        "and every file under `notes/` all agree."
    )
    lines.append("")
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--write", action="store_true", help=f"rewrite {README.relative_to(REPO)}")
    mode.add_argument("--check", action="store_true", help="fail if the tree disagrees with the registry/README")
    args = parser.parse_args(argv)

    try:
        rows = load_registry(REGISTRY)
    except RegistryError as exc:
        print(f"notes_index: {exc}", file=sys.stderr)
        return 2

    total_doctrine, doctrine_files = doctrine_bytes(rows, REPO)

    if args.write:
        # Mirrors cpp_tu_state.py's --write-note: render and write unconditionally.
        # README.md is itself a registry row (self-referential): its own doctrine
        # tag counts its own byte size, which changes when its content (and thus
        # its size) changes. Iterate to the fixed point (converges in 1-2 extra
        # passes in practice) rather than ever leave a slightly-stale README.
        rendered = render_readme(rows, REPO)
        for _ in range(5):
            README.write_text(rendered, encoding="utf-8", newline="\n")
            total_doctrine, doctrine_files = doctrine_bytes(rows, REPO)
            next_rendered = render_readme(rows, REPO)
            if next_rendered == rendered:
                break
            rendered = next_rendered
        else:
            print("notes_index: README did not converge after 5 passes", file=sys.stderr)
            return 2
        print(f"wrote {README.relative_to(REPO).as_posix()} "
              f"({len(rows)} files, doctrine {total_doctrine:,} bytes)")
        return 0

    errors = validate(rows, REPO)

    if args.check:
        rendered = render_readme(rows, REPO)
        actual = README.read_text(encoding="utf-8") if README.is_file() else ""
        if actual != rendered:
            errors.append(
                f"{README.relative_to(REPO).as_posix()} is stale; run "
                "`python tools/notes_index.py --write`"
            )
        if errors:
            for err in errors:
                print(f"notes_index: {err}", file=sys.stderr)
            print(f"notes_index: {len(errors)} problem(s)", file=sys.stderr)
            return 1
        over = total_doctrine - DOCTRINE_CAP_BYTES
        cap_note = (
            f"OVER by {over:,} bytes ({over / 1024:.1f}KB)" if over > 0
            else f"under cap by {-over:,} bytes"
        )
        print(
            f"notes_index: registry, README, and {len(rows)} notes/ files agree. "
            f"doctrine = {total_doctrine:,} bytes across {len(doctrine_files)} files, "
            f"cap = {DOCTRINE_CAP_BYTES:,} bytes ({cap_note}, not gated)"
        )
        return 0

    if errors:
        for err in errors:
            print(f"notes_index: {err}", file=sys.stderr)
        print(f"notes_index: {len(errors)} problem(s)", file=sys.stderr)
        return 1

    print(render_readme(rows, REPO), end="")
    return 0


if __name__ == "__main__":
    sys.exit(main())
