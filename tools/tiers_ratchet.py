#!/usr/bin/env python3
"""Backslide gate for the CONVERTED tier: a readable file may not quietly stop being one.

WHAT IT GATES. `tools/tiers.py` scores every source file against the five CONVERTED
criteria (real function name, no raw offset arithmetic, no `unk_<off>` fields, no
codegen tricks, no calls through mangled names). This tool banks the SET of file paths
that pass all five and fails a PR when a path LEAVES that set. It reuses tiers.score_file
outright -- the classifier has exactly one implementation, and a second copy of those
regexes would be a second definition of a published percentage.

WHY BACKSLIDE-ONLY, AND NOT A COUNT. Two reasons, and the second is the important one.

  A count ratchet ("converted may not fall") is satisfied by converting one file while
  wrecking another, which is the trade this gate exists to notice. A set ratchet names
  the file.

  More importantly, this project's goal ordering is not negotiable: a historically
  accurate C++ source that reproduces the ROM's exact bytes comes FIRST, readability
  third. Byte-match is never traded for readability. And the two genuinely collide --
  the tree's own measurements show that whether a raw cast can become a named member is
  decided PER FUNCTION by the byte gate, not by taste: migrating one function's 13 raw
  casts to real members was free, while another function in the same family paid 0x14
  bytes to the same edit because members over-CSE'd. So a future matching fix may
  legitimately have to put offset arithmetic BACK into a file this baseline calls
  CONVERTED. A gate with no way to say that would be blocking goal 1 to protect goal 3,
  which is exactly backwards.

  Hence the override below is a real, always-available mechanism, not a note saying one
  ought to exist. The gate's job is to make a backslide LOUD and ATTRIBUTED, not to make
  it impossible.

HOW TO OVERRIDE (the escape hatch)

    python tools/tiers_ratchet.py --update --reason "St_WallJump_Init: named members
        cost 0x14 bytes, reverted to offset casts to hold the match"

`--update` rewrites the baseline. It refuses -- and this is its ONLY refusal -- to
REMOVE a path without `--reason`. Each removed path is appended to
`config/converted-backslide-exceptions.jsonl` with that reason, so every file that ever
left the CONVERTED set is a line in a small file with a `git blame` and a PR behind it.
Additions never need a reason and never fail anything.

TWO FAILURE MODES OF `langmode_audit.py` THIS IS BUILT NOT TO REPEAT

  1. STUCK BOTH WAYS. That ratchet's `--update` can refuse because the tree "left the
     backlog by failing differently", which leaves no way forward except re-pinning from
     a clean origin/main checkout. Here, `--update` has no state-dependent refusal at
     all: given `--reason` when it removes, it always writes. There is no tree state from
     which this tool cannot be re-pinned in one command.

  2. AN ENVIRONMENT-SPECIFIC BASELINE. The langmode baseline lives on the `chaos-data`
     branch and its consumers need a second checkout to read it. This one is a plain JSON
     file in the tree at a fixed path, readable and writable with no git plumbing, and
     `--baseline PATH` points anywhere for a CI job or a test.

     In-tree is safe here in a way a COUNTER is not, because staleness only ever runs one
     way: `--check` fails on removals alone, so a baseline that has not caught up with
     newly converted files is permissive, never falsely red. No PR needs to re-bank in
     order to pass, so the baseline is not in every PR's diff and does not conflict the
     way a counter did. Re-bank it occasionally, on its own, to bank recent gains.

NOT WIRED INTO ANYTHING YET -- deliberately. Do not add it to `tools/hooks/pre-push`.
The langmode ratchet's pre-push wiring is the one that has been observed to get stuck,
and a local hook that a contributor cannot get past is worse than no gate. The
recommended next step is a CI-only job mirroring `.github/workflows/langmode-ratchet.yml`
(checkout, setup-python, run `--check`), on `pull_request` paths `src/**`,
`tools/tiers.py`, `tools/tiers_ratchet.py`, `config/converted-baseline.json`. Prove it
does not get stuck across a few weeks of real PRs before anyone discusses a hook.

Usage:
    python tools/tiers_ratchet.py                 # summary; no exit-code meaning
    python tools/tiers_ratchet.py --check         # exit 1 on any backslide
    python tools/tiers_ratchet.py --update        # re-bank (additions only)
    python tools/tiers_ratchet.py --update --reason "..."   # re-bank with removals
    python tools/tiers_ratchet.py --list          # the current CONVERTED paths

Exit codes: 0 ok, 1 backslide detected, 2 usage/configuration error (missing baseline,
removal without a reason). It compiles nothing and reads no ROM: pure source text over
git-tracked files, about two seconds on the whole tree.
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import tiers  # noqa: E402

BASELINE = REPO / "config" / "converted-baseline.json"
EXCEPTIONS = REPO / "config" / "converted-backslide-exceptions.jsonl"

NOTE = ("The CONVERTED file set, banked. tools/tiers_ratchet.py --check fails when a "
        "path here no longer passes all five criteria in tools/tiers.py. Removals need "
        "--reason and land in config/converted-backslide-exceptions.jsonl. Regenerate "
        "with `python tools/tiers_ratchet.py --update`.")


def tracked_sources():
    """Every git-tracked C/C++ file under src/, as repo-relative posix paths.

    tiers.converted() walks the filesystem instead, which is right for a MEASUREMENT --
    it should describe the tree in front of you. A GATE wants the tree git agrees on, so
    an untracked scratch file cannot enter the baseline and then "backslide" by being
    deleted. Both enumerations currently return the same 11,287 files.
    """
    out = subprocess.run(["git", "-C", str(REPO), "ls-files", "src"],
                         capture_output=True, text=True, check=True).stdout.split()
    return sorted(p for p in out if p.endswith((".c", ".cpp")))


def score(rel):
    """tiers.score_file for one repo-relative path, or None if it is unreadable."""
    try:
        text = (REPO / rel).read_text(errors="replace")
    except OSError:
        return None
    return tiers.score_file(rel, text)


def scan(paths=None):
    """(converted_set, scores_by_path) for the whole tracked tree."""
    scores = {}
    converted = set()
    for rel in (paths if paths is not None else tracked_sources()):
        s = score(rel)
        if s is None:
            continue
        scores[rel] = s
        if all(s[k] for k in tiers.CRITERIA):
            converted.add(rel)
    return converted, scores


def load_baseline(path):
    """The banked set, or None when there is no usable baseline file.

    A malformed or absent baseline is a configuration error, never an empty set: an
    empty set makes --check pass forever, so a deleted baseline would silently disable
    the gate rather than break it.
    """
    p = pathlib.Path(path)
    if not p.is_file():
        return None
    try:
        d = json.loads(p.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None
    got = d.get("converted")
    if not isinstance(got, list):
        return None
    return set(got)


def write_baseline(path, converted):
    p = pathlib.Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    body = {"_note": NOTE,
            "criteria": list(tiers.CRITERIA),
            "count": len(converted),
            "converted": sorted(converted)}
    with open(p, "w", encoding="utf-8", newline="\n") as f:
        f.write(json.dumps(body, indent=2) + "\n")


def append_exceptions(path, rows):
    p = pathlib.Path(path)
    p.parent.mkdir(parents=True, exist_ok=True)
    with open(p, "a", encoding="utf-8", newline="\n") as f:
        for r in rows:
            f.write(json.dumps(r, sort_keys=True) + "\n")


def why(rel, scores, tracked):
    """Why a banked path is no longer CONVERTED, in the words of the criteria."""
    if rel not in tracked:
        return "GONE -- not a tracked source file any more (deleted, renamed or moved)"
    s = scores.get(rel)
    if s is None:
        return "UNREADABLE -- the file could not be read"
    failed = [k for k in tiers.CRITERIA if not s[k]]
    if not failed:
        # Cannot happen through --check, which derives both sides from one scan; it can
        # happen if a caller passes a hand-edited path list, so say so rather than lie.
        return "no criterion fails -- baseline and scan disagree, re-run --check"
    return "; ".join(tiers.CRITERION_LABEL[k] for k in failed)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--check", action="store_true",
                    help="exit 1 if any banked path is no longer CONVERTED")
    ap.add_argument("--update", action="store_true",
                    help="rewrite the baseline from the current tree")
    ap.add_argument("--reason", metavar="TEXT",
                    help="required by --update when it would REMOVE a path; recorded "
                         "against every removed path in the exceptions log")
    ap.add_argument("--date", metavar="YYYY-MM-DD",
                    help="optional date to stamp on exception rows. Omitted by default "
                         "on purpose: git already dates the commit that adds the row, "
                         "and a live clock would make this tool untestable")
    ap.add_argument("--list", action="store_true",
                    help="print the current CONVERTED paths, one per line")
    ap.add_argument("--baseline", default=str(BASELINE), metavar="PATH")
    ap.add_argument("--exceptions", default=str(EXCEPTIONS), metavar="PATH")
    args = ap.parse_args()

    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    tracked = tracked_sources()
    current, scores = scan(tracked)
    tracked_set = set(tracked)

    if args.list:
        print("\n".join(sorted(current)))
        return 0

    banked = load_baseline(args.baseline)

    if args.update:
        removed = sorted((banked or set()) - current)
        added = sorted(current - (banked or set()))
        if removed and not args.reason:
            print(f"REFUSING to bank {len(removed)} removal(s) without --reason:\n")
            for rel in removed:
                print(f"  {rel}\n      {why(rel, scores, tracked_set)}")
            print("\nA path leaving the CONVERTED set is allowed -- byte-match outranks\n"
                  "readability and sometimes requires it -- but it is not allowed to be\n"
                  "silent. Re-run with --reason \"<why the match needed it>\"; the reason\n"
                  f"is appended to {args.exceptions} for every path above.")
            return 2
        if removed:
            append_exceptions(args.exceptions,
                              [dict({"path": rel, "reason": args.reason},
                                    **({"date": args.date} if args.date else {}))
                               for rel in removed])
        write_baseline(args.baseline, current)
        if banked is None:
            print(f"baked {len(current)} CONVERTED path(s) into {args.baseline} (new baseline)")
        else:
            print(f"wrote {args.baseline}: {len(banked)} -> {len(current)} "
                  f"(+{len(added)} / -{len(removed)})")
            if removed:
                print(f"logged {len(removed)} removal(s) to {args.exceptions}")
        return 0

    if args.check:
        if banked is None:
            print(f"no usable baseline at {args.baseline}.\n"
                  "Bake one with `python tools/tiers_ratchet.py --update`. An absent\n"
                  "baseline is a configuration error, not an empty set: treating it as\n"
                  "empty would make this gate pass forever.")
            return 2
        missing = sorted(banked - current)
        gained = len(current - banked)
        if missing:
            print(f"CONVERTED backslide: {len(missing)} banked file(s) no longer pass "
                  f"all {len(tiers.CRITERIA)} criteria\n")
            for rel in missing:
                print(f"  {rel}\n      {why(rel, scores, tracked_set)}")
            print(f"\nbaseline {len(banked)}   current {len(current)}   "
                  f"(+{gained} gained, -{len(missing)} lost)")
            print("\nIf a byte match REQUIRED this -- and it legitimately can; raw-cast\n"
                  "versus named member is decided per function by the byte gate -- bank\n"
                  "it with a reason instead of reverting readable code:\n"
                  '  python tools/tiers_ratchet.py --update --reason "<why>"')
            return 1
        tail = f"   (+{gained} gained, not yet banked)" if gained else ""
        print(f"CONVERTED ratchet PASS   baseline {len(banked)}   "
              f"current {len(current)}{tail}")
        return 0

    # No mode flag: a plain report. Says the same things --check would, without an
    # exit code, so it is safe to run while poking at the tree.
    print(f"tracked src files      {len(tracked):6d}")
    print(f"CONVERTED now          {len(current):6d}   "
          f"{100.0 * len(current) / len(tracked):.2f}%")
    if banked is None:
        print(f"baseline               (none at {args.baseline}; run --update)")
        return 0
    missing = sorted(banked - current)
    print(f"baseline               {len(banked):6d}   {args.baseline}")
    print(f"gained, not banked     {len(current - banked):6d}")
    print(f"BACKSLID               {len(missing):6d}"
          f"{'   <- --check would fail' if missing else ''}")
    for rel in missing[:20]:
        print(f"    {rel}\n        {why(rel, scores, tracked_set)}")
    if len(missing) > 20:
        print(f"    ... and {len(missing) - 20} more")
    return 0


if __name__ == "__main__":
    sys.exit(main())
