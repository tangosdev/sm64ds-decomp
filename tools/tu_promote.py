#!/usr/bin/env python3
"""Enroll a text-verified manifest TU into the production ROM build.

Promotion changes NOTHING about codegen. Text-verification already proved every
function's bytes and relocations from a single compile of the reconstructed source;
promotion only changes which object the linker takes them from -- one TU instead of
N per-function objects. That is the entire point: the cartridge's own build emitted
each class's constructors and destructors from one translation unit, and while the
tree carries a file per mangled symbol, every structor has to be hand-placed under a
``// @symbol`` marker. Promoting the TU hands that job back to the compiler.

The mechanical steps, all of which this performs and none of which it guesses at:

* replace the entry's per-function ``delinks.txt`` entries with one ``complete``
  entry spanning every manifest-owned section;
* ``git mv`` the ``src_tu/`` source to its ``promoted_source`` path (R100, so the
  file's own credit follows) and ``git rm`` every ``legacy_source``;
* rewrite the manifest entry to ``status: promoted`` with ``source`` at the
  production path, matching the entries already enrolled;
* add one ``attribution.json`` override per absorbed symbol, so a many-to-one
  consolidation reads as "consolidated with credit intact" instead of N lost.
* migrate banked CONVERTED legacy paths to ``promoted-path#symbol`` identities,
  preserving the readability ratchet at function rather than physical-file granularity.

It deliberately does NOT compile anything. It preflights the tracked full-ROM proof;
after promotion, refresh the content-bound stock control, then require ``rombuild.py``
to reproduce that stock ROM and report 106/106 with ``mismatching: 0`` plus the final
linked-symbol/address-point gates. Refusals here need no candidate compile.
"""

from __future__ import annotations

import argparse
import copy
import json
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import tu_manifest as TUM  # noqa: E402
import tiers_ratchet as TR  # noqa: E402
import rombuild as RB  # noqa: E402
import tubuild as TB  # noqa: E402

CONFIG = REPO / "config"


class PromoteError(RuntimeError):
    """A promotion was refused on a fact that needs no compiler to check."""


def delinks_path(module):
    """The delinks.txt for a manifest module name ("arm9" or "ovNNN")."""
    if module in ("arm9", "main"):
        return CONFIG / "arm9" / "delinks.txt"
    return CONFIG / "arm9" / "overlays" / module / "delinks.txt"


def plan(entry):
    """Everything the promotion will touch, or a PromoteError explaining why not."""
    ident = entry.get("id", "<unknown>")
    if entry.get("status") == "promoted":
        raise PromoteError(f"{ident}: already promoted")
    src = entry.get("source", "")
    dest = entry.get("promoted_source", "")
    if not src.startswith("src_tu/") or not dest.startswith("src/"):
        raise PromoteError(f"{ident}: expected src_tu/ source and src/ promoted_source, "
                           f"got {src!r} -> {dest!r}")
    # MEASURED, not defensive: `ChillBully+daIDonketu_c.cpp` linked fine as a src_tu
    # compile and then broke the whole ROM at the link --
    #   mwldarm.exe: File not found: ChillBully
    #   mwldarm.exe: Expecting: (
    # because dsd writes bare, unquoted object names into the LCF and mwldarm's parser
    # treats `+` as punctuation. src_tu/ names a multi-class TU with `+` and nothing
    # ever links those, so the convention is safe exactly until promotion. No promoted
    # source has ever carried one; keep it that way here, where it costs a second.
    if not re.fullmatch(r"[A-Za-z0-9_./-]+", dest):
        raise PromoteError(f"{ident}: {dest!r} has a character the linker command file "
                           f"cannot carry unquoted; rename the file before promoting")
    if not (REPO / src).is_file():
        raise PromoteError(f"{ident}: {src} is not on disk")
    if (REPO / dest).exists():
        raise PromoteError(f"{ident}: {dest} already exists")

    normalized_claims, claim_reasons = TB.manifest_section_claims(entry)
    if claim_reasons:
        raise PromoteError(f"{ident}: invalid manifest section claims: "
                           + "; ".join(claim_reasons))
    owns_nontext = any(s["name"] != ".text" for s in normalized_claims) \
        or bool(entry.get("data") or entry.get("rodata") or entry.get("bss"))
    if owns_nontext and entry.get("production_mode") != "intact-object":
        # Production isolation zeroes an object's data and rebinds the symbols to the
        # cartridge's addresses unless the normal build has explicitly admitted this
        # manifest-backed intact-object policy.
        raise PromoteError(f"{ident}: entry owns non-text data without "
                           "production_mode: intact-object")

    claims = []
    spans = []
    for section in normalized_claims:
        name = section["name"]
        module_section = section["module_section"]
        lo, hi = section["start"], section["end"]
        if (entry.get("production_mode") == "intact-object"
                and module_section != name):
            raise PromoteError(f"{ident}: intact-object claim {name} -> "
                               f"{module_section} needs input-section retargeting, "
                               "which is not implemented")
        claims.append((module_section, lo, hi))
        if name == ".text":
            spans.append((lo, hi))

    funcs = entry.get("functions", [])
    if not funcs:
        raise PromoteError(f"{ident}: entry licenses no functions")
    legacy = []
    for f in funcs:
        addr, size = int(f["address"], 16), int(f["size"], 16)
        if not any(lo <= addr and addr + size <= hi for lo, hi in spans):
            raise PromoteError(f"{ident}: {f['symbol']} at 0x{addr:08x}+0x{size:x} "
                               f"falls outside the manifest's .text span(s)")
        source = f.get("legacy_source")
        if not source:
            raise PromoteError(f"{ident}: {f['symbol']} has no legacy_source")
        if source not in legacy:
            legacy.append(source)

    dl = delinks_path(entry["module"])
    if not dl.is_file():
        raise PromoteError(f"{ident}: {dl} does not exist")
    if len(spans) != 1:
        raise PromoteError(f"{ident}: production promotion needs exactly one .text "
                           f"span, got {len(spans)}")
    _header, _entries, _inside, _validated_claims, splice_reasons = \
        TB.validate_tu_entry_splice(dl, spans[0][0], spans[0][1], dest, legacy,
                                    normalized_claims)
    if splice_reasons:
        raise PromoteError(f"{ident}: current delinks ownership is not safe to splice: "
                           + "; ".join(splice_reasons))
    if entry.get("production_mode") == "intact-object":
        prospective = copy.deepcopy(entry)
        prospective["status"] = "promoted"
        prospective["source"] = dest
        try:
            RB.intact_tu_policies([dest], manifest={"entries": [prospective]})
        except RB.BuildError as exc:
            raise PromoteError(f"{ident}: production admission preflight failed: "
                               f"{exc.output}") from exc

    return {"id": ident, "module": entry["module"], "source": src, "dest": dest,
            "delinks": dl,
            "legacy": legacy, "spans": spans, "claims": claims,
            "section_claims": normalized_claims, "functions": funcs}


def rewrite_delinks(p):
    """N per-function entries out, one spanning entry in, using the shared gate."""
    replaced, reasons = TB.splice_tu_entry(
        p["delinks"], p["spans"][0][0], p["spans"][0][1], p["dest"],
        p["legacy"], p["section_claims"])
    if reasons:
        raise PromoteError(f"{p['id']}: delinks ownership changed after preflight: "
                           + "; ".join(reasons))
    if set(replaced) != set(p["legacy"]):
        raise PromoteError(f"{p['id']}: shared splice replaced {replaced}, expected "
                           f"{p['legacy']}")


def rewrite_manifest(entry, p):
    entry["source"] = p["dest"]
    entry["status"] = "promoted"
    path = CONFIG / "tu_manifest.d" / f"{entry['id']}.json"
    path.write_text(json.dumps(entry, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="")


def attribution_update(plans, lineage):
    """Prepare attribution overrides without changing the worktree.

    Without these, prepush_attribution reports every legacy basename as CREDIT LOST
    and the merge gate needs a label to pass -- for a change that took nothing away
    from anyone.
    """
    path = REPO / "attribution.json"
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise PromoteError(f"attribution data is unreadable: {exc}") from exc
    ov = data.setdefault("overrides", {})
    if not isinstance(ov, dict):
        raise PromoteError("attribution overrides must be an object")
    added = 0
    for p in plans:
        for f in p["functions"]:
            stem = pathlib.PurePosixPath(f["legacy_source"]).stem
            who = lineage.get(f"src/{stem}")
            if not who:
                continue
            key = f"{p['dest']}#{f['symbol']}"
            if key not in ov:
                ov[key] = who
                added += 1
    return path, data, added


def rewrite_attribution(plans, lineage, prepared=None):
    """Write a preflighted attribution update."""
    path, data, added = prepared or attribution_update(plans, lineage)
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="")
    return added


def converted_baseline_update(plans):
    """Prepare a CONVERTED identity rewrite without changing the worktree.

    The baseline historically keyed one-function intake by path. Consolidation deletes
    those paths, while ``tiers.py`` continues counting the functions inside the promoted
    TU. Only already-banked legacy identities move; an unbanked function does not become
    readable merely because it now shares a file with one that was.
    """
    path = CONFIG / "converted-baseline.json"
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        raise PromoteError(f"converted baseline is unreadable: {exc}") from exc
    rows = data.get("converted")
    if not isinstance(rows, list) or len(rows) != len(set(rows)):
        raise PromoteError("converted baseline must contain a unique converted list")

    converted = list(rows)
    moved = 0
    for p in plans:
        for f in p["functions"]:
            legacy = f["legacy_source"]
            symbol = f["symbol"]
            old_keys = (legacy, f"{legacy}#{symbol}")
            positions = [converted.index(key) for key in old_keys if key in converted]
            if not positions:
                continue
            insert_at = min(positions)
            converted = [key for key in converted if key not in old_keys]
            target = p["dest"] if len(p["functions"]) == 1 \
                else f"{p['dest']}#{symbol}"
            if target not in converted:
                converted.insert(min(insert_at, len(converted)), target)
            moved += 1

    data["_note"] = TR.NOTE
    data["count"] = len(converted)
    data["converted"] = converted
    return path, data, moved


def rewrite_converted_baseline(plans, prepared=None):
    """Write a preflighted CONVERTED identity rewrite."""
    path, data, moved = prepared or converted_baseline_update(plans)
    path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n",
                    encoding="utf-8", newline="")
    return moved


def git(*args):
    subprocess.run(["git", *args], cwd=REPO, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)


def batch_preflight(plans):
    """Refuse a promotion batch before its first mutation if ownership is ambiguous."""
    errors = []
    destinations = {}
    consumed = {}
    claims_by_delinks = {}
    required = set()
    for p in plans:
        dest = p["dest"]
        if dest in destinations:
            errors.append(f"{dest} is the destination of both {destinations[dest]} "
                          f"and {p['id']}")
        destinations[dest] = p["id"]
        inputs = [("shadow source", p["source"])]
        inputs.extend(("legacy source", rel) for rel in p["legacy"])
        for role, rel in inputs:
            required.add(rel)
            owner = consumed.get(rel)
            if owner and owner != p["id"]:
                errors.append(f"{rel} is consumed by both {owner} and {p['id']}")
            consumed[rel] = p["id"]
            if not (REPO / rel).is_file():
                errors.append(f"{p['id']}: {role} {rel} is not on disk")
        owned = claims_by_delinks.setdefault(str(p["delinks"]), [])
        for section, start, end in p.get("claims", []):
            for other_start, other_end, other_id, other_section in owned:
                if max(start, other_start) < min(end, other_end):
                    errors.append(
                        f"{p['id']} {section} 0x{start:08x}..0x{end:08x} overlaps "
                        f"{other_id} {other_section} 0x{other_start:08x}.."
                        f"0x{other_end:08x} in the same delinks file")
            owned.append((start, end, p["id"], section))
    for dest, owner in destinations.items():
        if dest in consumed:
            errors.append(f"{owner}: destination {dest} is also a source consumed "
                          f"by {consumed[dest]}")

    if errors:
        raise PromoteError("batch preflight failed: " + "; ".join(errors))

    for rel in sorted(required):
        tracked = subprocess.run(
            ["git", "ls-files", "--error-unmatch", "--", rel], cwd=REPO,
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL).returncode == 0
        if not tracked:
            errors.append(f"{rel} is not tracked by git")
    if errors:
        raise PromoteError("batch preflight failed: " + "; ".join(errors))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("ids", nargs="+", help="manifest entry id(s), e.g. ov045/PoleLift")
    ap.add_argument("--dry-run", action="store_true",
                    help="report what each promotion would touch and change nothing")
    args = ap.parse_args()

    data = TUM.load()
    by_id = {e["id"]: e for e in data["entries"]}
    plans, entries, refused = [], [], []
    for ident in args.ids:
        entry = by_id.get(ident)
        if entry is None:
            refused.append(f"{ident}: no such manifest entry")
            continue
        try:
            plans.append(plan(entry))
            entries.append(entry)
        except PromoteError as exc:
            refused.append(str(exc))

    for why in refused:
        print(f"  refused  {why}")
    if refused or not plans:
        print("tu_promote: nothing to promote.")
        return 1 if refused else 0

    try:
        batch_preflight(plans)
    except PromoteError as exc:
        print(f"  refused  {exc}")
        print("tu_promote: nothing to promote.")
        return 1

    for p in plans:
        print(f"  promote  {p['id']:38s} {len(p['functions'])} function(s), "
              f"{len(p['legacy'])} legacy source(s) -> {p['dest']}")
    try:
        converted_update = converted_baseline_update(plans)
    except PromoteError as exc:
        print(f"  refused  {exc}")
        return 1
    if args.dry_run:
        print(f"tu_promote: {len(plans)} entry(ies) would be promoted and "
              f"{converted_update[2]} CONVERTED member identity/identities retained "
              "(dry run).")
        return 0

    import prepush_attribution as PA
    lineage = PA.lineage("HEAD")
    try:
        attribution = attribution_update(plans, lineage)
    except PromoteError as exc:
        print(f"  refused  {exc}")
        return 1
    for p, entry in zip(plans, entries):
        rewrite_delinks(p)
        # `git mv` will not create the destination directory, and a promoted_source
        # may name a directory under src/ that does not exist yet (as LevelObjects does).
        (REPO / p["dest"]).parent.mkdir(parents=True, exist_ok=True)
        git("mv", p["source"], p["dest"])
        for source in p["legacy"]:
            git("rm", "-q", source)
        rewrite_manifest(entry, p)
    converted = rewrite_converted_baseline(plans, converted_update)
    added = rewrite_attribution(plans, lineage, attribution)
    print(f"tu_promote: {len(plans)} entry(ies) promoted, "
          f"{sum(len(p['functions']) for p in plans)} function(s) consolidated, "
          f"{added} attribution override(s) added, "
          f"{converted} CONVERTED member identity/identities retained.")
    print("tu_promote: now refresh the content-bound control with "
          f"`python tools/tubuild.py linkcheck --baseline --module "
          f"{plans[0]['module']} -j16 --clean`, then run "
          "`python tools/rombuild.py -j16` -- 106/106, mismatching 0, zero new "
          "symbol errors, exact address points, and a stock-identical ROM are the proof.")
    return 1 if refused else 0


if __name__ == "__main__":
    sys.exit(main())
