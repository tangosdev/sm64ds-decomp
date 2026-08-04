"""Assert the invariants that make `src/` trustworthy, not merely tidy.

WHY THIS IS NOT COSMETIC
------------------------
One of these checks guards a failure the ROM build cannot catch. `enroll` writes each
source's path into `config/**/delinks.txt`, and `dsd` supplies retail ROM bytes for every
range it has no object for. So if a file moves and its delinks entry is not updated, the
function silently falls back to ROM bytes -- and the build still reports a byte-identical
ROM, because ROM bytes are correct by construction. Nothing goes red. The only visible
trace is the source-built function count ticking down by one, which is exactly the kind of
number nobody diffs.

That is L1 below, and it is the reason this file exists. The rest are ordinary tidiness,
with one exception: L4 is tidiness that silently disables `srcpath.placement_for`, so
letting it rot costs future placement rather than a match.

Checks
------
  L1  stale path    a delinks.txt names src/X but no file is there      ERROR
  L2  ambiguous     two source files share a stem                       ERROR
  L3  misfiled      src/unnamed/<mod>/ holds a symbol that is not an
                    address-named symbol of <mod>                       ERROR
  L4  split class   one class occupies two or more subdirectories       ERROR
  L5  unenrolled    a source file no delinks.txt mentions               INFO

L5 is informational on purpose: a matched function can be legitimately un-enrolled (thumb,
misaligned stub, on the exclude list), and `enroll` already reports those with reasons.
Counting them here is a trend line, not a gate.

Usage:
    python tools/layout_check.py            # human summary, exit 1 on any ERROR
    python tools/layout_check.py --json     # machine-readable
    python tools/layout_check.py --quiet    # only failures
"""
import argparse
import collections
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent
CONFIG = REPO / "config"
KNOWN = CONFIG / "layout-known-issues.txt"

_ENTRY_RE = re.compile(r"^(\S+):\s*$")


def known_issues(path=None):
    """{(check, key)} the tree is already carrying. See config/layout-known-issues.txt.

    A gate that fails on the day it lands teaches people to pass --no-verify. Recording
    the existing debt keeps the gate real for anything new, and makes burning the debt
    down a visible, reviewable diff rather than a silent allowance."""
    p = path or KNOWN
    out = set()
    if p.is_file():
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.split("#", 1)[0].strip()
            if line:
                code, _, key = line.partition(" ")
                out.add((code.strip(), key.strip()))
    return out


def delinks_paths(config=None):
    """{repo-relative src path: delinks file that names it} across every module."""
    out = {}
    for dl in sorted((config or CONFIG).rglob("delinks.txt")):
        for line in dl.read_text(encoding="utf-8", errors="replace").splitlines():
            m = _ENTRY_RE.match(line)
            if m and m.group(1).startswith("src/"):
                out[m.group(1)] = dl
    return out


def check(config=None, known=None):
    """Every invariant, as {code: [finding, ...]}. Empty lists are the happy path.

    Each finding carries a ``key``; findings whose (code, key) is in the known-issues
    list are moved to ``waived`` rather than dropped, so they stay countable."""
    known = known_issues() if known is None else known
    findings = collections.defaultdict(list)
    sources = list(SP.iter_sources())
    on_disk = {p.relative_to(SP.REPO).as_posix() for p in sources}
    enrolled = delinks_paths(config)

    # L1 -- the one the ROM build cannot catch.
    for rel, dl in sorted(enrolled.items()):
        if rel not in on_disk:
            where = SP.path_for(pathlib.PurePosixPath(rel).stem)
            findings["L1"].append({
                "key": rel,
                "delinks": dl.relative_to(REPO).as_posix(),
                "actually_at": where.relative_to(SP.REPO).as_posix() if where else None,
            })

    # L2 -- srcpath has to pick one, and the build uses whichever delinks names.
    by_stem = collections.defaultdict(list)
    for p in sources:
        by_stem[p.stem].append(p.relative_to(SP.REPO).as_posix())
    for stem, paths in sorted(by_stem.items()):
        if len(paths) > 1:
            findings["L2"].append({"key": stem, "paths": sorted(paths)})

    # L3 -- src/unnamed/<mod>/ is a module bucket; anything else in it is misfiled.
    unnamed_root = SP.SRC / SP.UNNAMED_DIR
    for p in sources:
        try:
            rest = p.relative_to(unnamed_root)
        except ValueError:
            continue
        if len(rest.parts) < 2:
            continue
        bucket = rest.parts[0]
        mod = SP.module_of(p.stem)
        if mod is None:
            findings["L3"].append({"key": p.relative_to(SP.REPO).as_posix(),
                                   "bucket": bucket, "why": "symbol carries a name, not an address"})
        elif mod != bucket:
            findings["L3"].append({"key": p.relative_to(SP.REPO).as_posix(),
                                   "bucket": bucket, "why": f"symbol belongs to {mod}"})

    # L4 -- two homes for one class is what makes placement_for give up.
    dirs = collections.defaultdict(set)
    for p in sources:
        cls = SP.class_of(p.stem)
        if cls and p.parent != SP.SRC:
            dirs[cls].add(p.parent.relative_to(SP.SRC).as_posix())
    for cls, ds in sorted(dirs.items()):
        if len(ds) > 1:
            findings["L4"].append({"key": cls, "dirs": sorted(ds)})

    # L5 -- informational.
    for rel in sorted(on_disk - set(enrolled)):
        findings["L5"].append({"key": rel})

    # Always emit every code, empty or not: a caller asking for r["L4"] wants "no split
    # classes", not a KeyError that reads like the check never ran.
    out, waived = {}, []
    for code in ERRORS + ("L5",):
        keep = []
        for h in findings.get(code, []):
            if code in ERRORS and (code, h["key"]) in known:
                waived.append(dict(h, check=code))
            else:
                keep.append(h)
        out[code] = keep
    out["waived"] = waived
    return out


ERRORS = ("L1", "L2", "L3", "L4")
LABEL = {
    "L1": "delinks names a path with no file there (function silently falls back to ROM bytes)",
    "L2": "two source files share a symbol",
    "L3": "misfiled under src/unnamed/",
    "L4": "one class split across two directories (disables placement for it)",
    "L5": "source file not enrolled in any delinks.txt",
}


def print_report(findings, quiet=False):
    failed = False
    for code in ERRORS + ("L5",):
        hits = findings.get(code, [])
        bad = code in ERRORS and hits
        failed = failed or bool(bad)
        if quiet and not bad:
            continue
        mark = "FAIL" if bad else ("info" if code == "L5" else "ok  ")
        print(f"[{mark}] {code} {LABEL[code]}: {len(hits)}")
        for h in hits[:10]:
            print(f"         {json.dumps(h, sort_keys=True)}")
        if len(hits) > 10:
            print(f"         ... and {len(hits) - 10} more")
    waived = findings.get("waived", [])
    if waived and not quiet:
        print(f"[note] {len(waived)} pre-existing violation(s) waived by "
              f"config/layout-known-issues.txt "
              f"({', '.join(sorted(set(w['check'] for w in waived)))})")
    print("layout-check: " + ("FAILED" if failed else "clean"))
    return failed


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--json", action="store_true", help="emit findings as JSON")
    ap.add_argument("--quiet", action="store_true", help="print only failing checks")
    a = ap.parse_args()
    findings = check()
    if a.json:
        print(json.dumps(findings, indent=1, sort_keys=True))
        return 1 if any(findings.get(c) for c in ERRORS) else 0
    return 1 if print_report(findings, a.quiet) else 0


if __name__ == "__main__":
    sys.exit(main())
