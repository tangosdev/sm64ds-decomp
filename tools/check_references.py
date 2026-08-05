"""Fail when a change adds a reference that resolves to nothing.

## Why this exists

`match.py` compares a compiled function to the ROM word by word, but every
relocated word is a **wildcard** -- our object has a placeholder where the ROM has
a final address. So the byte gate compares "is there a call here", never "a call to
what". A file can name a callee that does not exist and still match.

The ROM link *would* object. But `eligible.py` refuses to enroll a file whose
references do not resolve, so a file with a bad reference never reaches the link.
The two gates cover for each other, and the gap between them is where every
reference bug in this tree has lived:

    882 refs   C++ TUs re-mangling ROM names they declared without C linkage
    250 files  `G0`/`VT1`/`HEAP` placeholders a recovery pass never resolved
    196 files  `_ZTV10dBgActor_c` -- one plausible-looking name, 196 real symbols
    106 refs   mangled names built from mis-recovered signatures
      3 funcs  outright wrong callees, found only once enrollment forced a link

Each was cleaned up by a sweep over the whole tree. Sweeps remove instances; they
do not remove the hole that admits them. This closes the hole: a reference that
resolves to nothing can be *added* only if this check is not run.

## What it checks

`eligible.py` already computes the answer -- it compiles every candidate and records
each file's unresolved symbols in `build/rombuild-eligibility.json`. This reads that
report and compares it to a committed baseline, failing on:

  - a file that resolves today and does not in the report  (regression)
  - an unresolved symbol name the baseline has never seen  (a new bug class)

It deliberately does **not** fail on the baseline's own backlog. The tree has a real
one; blocking every PR until it is empty would just get the check switched off.
Progress ratchets the baseline down, never up.

Usage:
    python tools/check_references.py                 # verify against the baseline
    python tools/check_references.py --update        # ratchet the baseline down
    python tools/check_references.py --report FILE   # use a report from elsewhere
"""
import argparse
import json
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
REPORT = REPO / "build" / "rombuild-eligibility.json"
BASELINE = REPO / "config" / "unresolved-baseline.json"


def load_report(path):
    if not path.is_file():
        sys.exit(f"missing {path}\nRun tools/eligible.py first, or pass --report.")
    out = {}
    for r in json.loads(path.read_text(encoding="utf-8")):
        if (r.get("reason") or "").startswith("unresolvable"):
            out[r["file"].replace("\\", "/")] = sorted(set(r.get("missing") or []))
    return out


def load_baseline():
    if not BASELINE.is_file():
        return None
    return json.loads(BASELINE.read_text(encoding="utf-8"))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--update", action="store_true",
                    help="write the current state as the baseline (only ever downward)")
    ap.add_argument("--report", type=pathlib.Path, default=REPORT)
    args = ap.parse_args()

    cur = load_report(args.report)
    cur_names = {s for v in cur.values() for s in v}

    if args.update or load_baseline() is None:
        base = load_baseline()
        if not args.update:
            sys.exit(f"no baseline at {BASELINE}\nCreate one with --update.")
        # Only ratchet downward -- a baseline that can grow is not a baseline. The
        # first write is the exception: there is nothing yet to ratchet against.
        grew = [f for f in cur if f not in base["files"]] if base else []
        new_names = sorted(cur_names - set(base["names"])) if base else []
        if grew or new_names:
            print("refusing to update: this would raise the baseline, not lower it")
            for f in grew[:10]:
                print(f"  newly unresolved file: {f}")
            for n in new_names[:10]:
                print(f"  new unresolved name  : {n}")
            return 1
        BASELINE.parent.mkdir(parents=True, exist_ok=True)
        BASELINE.write_text(json.dumps(
            {"files": {f: v for f, v in sorted(cur.items())},
             "names": sorted(cur_names)}, indent=1) + "\n", encoding="utf-8")
        print(f"baseline updated: {len(cur)} files, {len(cur_names)} distinct names")
        return 0

    base = load_baseline()
    base_files, base_names = base["files"], set(base["names"])

    regressed = sorted(set(cur) - set(base_files))
    new_names = sorted(cur_names - base_names)
    fixed = sorted(set(base_files) - set(cur))

    print(f"unresolved files: {len(cur)}  (baseline {len(base_files)})")
    if fixed:
        print(f"  {len(fixed)} fixed since the baseline -- run --update to bank it")

    if not regressed and not new_names:
        print("OK: no new unresolvable references")
        return 0

    print("\nFAIL: this change adds references that resolve to nothing.")
    print("Every one of them would pass the byte gate, because match.py compares")
    print("relocated words as wildcards and never looks at what a call targets.\n")
    for f in regressed[:25]:
        print(f"  {f}\n      {', '.join(cur[f][:4])}")
    if len(regressed) > 25:
        print(f"  ... and {len(regressed) - 25} more files")
    for n in new_names[:15]:
        print(f"  new unresolved name: {n}")
    print("\nResolve them from the ROM's own relocation data:")
    print("  python tools/resolve_placeholders.py            # report")
    print("  python tools/resolve_placeholders.py --apply    # rewrite + byte-verify")
    return 1


if __name__ == "__main__":
    sys.exit(main())
