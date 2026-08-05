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
do not remove the hole that admits them. This closes the hole.

## What it compares, and why keyed that way

`eligible.py` already computes the answer -- it compiles every candidate and records
each file's unresolved symbols. This reads that report and compares it to
`config/unresolved-baseline.json`.

The key is the **function symbol**, never the file path. This project moves files
between directories and keys contributor lineage on those moves; a path key would
report every move as a regression and then refuse to let `--update` fix it.

The comparison is **per symbol's full missing-name list**, not a set of keys. An
earlier version compared only which functions appeared, plus a global set of names,
and so passed a change that added a *second* bad reference to a function that was
already in the backlog, as long as the name occurred somewhere else in the tree.

Three ways a check like this fails open, all closed here:

  - **stale report.** `eligible.py` stamps the commit it describes; a report from a
    different commit, or from a dirty tree, is refused rather than trusted.
  - **reason-switching.** A file that degrades from `unresolvable` to `compile
    failed` leaves the unresolved set and would read as a fix. The eligible count is
    ratcheted too, so trading one failure for another cannot pass.
  - **a hand-raised baseline.** `--update` only ever lowers it, and `--against`
    compares to the base branch's copy so raising it in a PR is visible.

The existing backlog is tolerated on purpose. The tree has a real one; blocking
every PR until it is empty would just get the check switched off.

Usage:
    python tools/check_references.py                      # verify
    python tools/check_references.py --update             # bank progress
    python tools/check_references.py --against origin/main
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import eligible as E  # noqa: E402

BASELINE = REPO / "config" / "unresolved-baseline.json"


def current(path, require_fresh=True):
    """{symbol: [missing names]} plus the eligible count, from the report."""
    try:
        entries, commit, dirty = E.load_report(path)
    except FileNotFoundError:
        sys.exit(f"missing {path}\nRun tools/eligible.py first, or pass --report.")
    if require_fresh:
        head = subprocess.run(["git", "rev-parse", "HEAD"], cwd=REPO,
                              capture_output=True, text=True).stdout.strip()
        if commit is None:
            sys.exit("report carries no commit stamp -- re-run tools/eligible.py")
        if commit != head:
            sys.exit(f"report describes {commit[:12]}, HEAD is {head[:12]}\n"
                     "A stale report would let a bad reference through. Re-run "
                     "tools/eligible.py.")
        if dirty:
            sys.exit("report was produced from a dirty tree -- commit or stash, then "
                     "re-run tools/eligible.py")
    unresolved, eligible, reasons = {}, 0, {}
    for r in entries:
        reason = r.get("reason")
        reasons[r["name"]] = reason
        if reason is None:
            eligible += 1
        elif reason.startswith("unresolvable"):
            unresolved[r["name"]] = sorted(set(r.get("missing") or []))
    return unresolved, eligible, reasons


def load_baseline(ref=None):
    if ref:
        out = subprocess.run(["git", "show", f"{ref}:{BASELINE.relative_to(REPO).as_posix()}"],
                             cwd=REPO, capture_output=True, text=True)
        return json.loads(out.stdout) if out.returncode == 0 else None
    return json.loads(BASELINE.read_text(encoding="utf-8")) if BASELINE.is_file() else None


def write_baseline(unresolved, eligible):
    BASELINE.parent.mkdir(parents=True, exist_ok=True)
    BASELINE.write_text(json.dumps(
        {"eligible": eligible, "unresolved": dict(sorted(unresolved.items()))},
        indent=1) + "\n", encoding="utf-8")


def worse(cur, base):
    """Symbols that resolve worse than the baseline allows."""
    out = []
    for sym, miss in sorted(cur.items()):
        was = base.get(sym)
        if was is None:
            out.append((sym, miss, "newly unresolvable"))
        else:
            added = sorted(set(miss) - set(was))
            if added:
                out.append((sym, added, "new unresolved reference"))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--update", action="store_true", help="bank progress (downward only)")
    ap.add_argument("--against", help="git ref whose baseline to compare against")
    ap.add_argument("--report", type=pathlib.Path,
                    default=REPO / "build" / "rombuild-eligibility.json")
    ap.add_argument("--allow-stale", action="store_true",
                    help="skip the commit-stamp check (for local iteration only)")
    args = ap.parse_args()

    cur, eligible, reasons = current(args.report, require_fresh=not args.allow_stale)
    base = load_baseline(args.against)

    if args.update:
        if base is not None:
            bad = worse(cur, base["unresolved"])
            regressed = [s for s in set(base["unresolved"]) - set(cur)
                         if reasons.get(s, "gone") is not None]
            if regressed:
                print("refusing to update: symbols left the backlog by failing differently")
                for s in regressed[:10]:
                    print(f"  {s} -> {reasons.get(s, 'no longer a candidate')}")
                return 1
            if bad or eligible < base.get("eligible", 0):
                print("refusing to update: this would raise the baseline, not lower it")
                for sym, names, why in bad[:10]:
                    print(f"  {why}: {sym}  ({', '.join(names[:3])})")
                if eligible < base.get("eligible", 0):
                    print(f"  eligible fell {base['eligible']} -> {eligible}")
                return 1
        write_baseline(cur, eligible)
        print(f"baseline: {len(cur)} unresolved symbols, {eligible} eligible")
        return 0

    if base is None:
        sys.exit(f"no baseline at {BASELINE}\nCreate one with --update.")

    bad = worse(cur, base["unresolved"])
    lost = base.get("eligible", 0) - eligible
    left = set(base["unresolved"]) - set(cur)
    # Leaving the unresolved set is only progress if the symbol became eligible. A
    # file that degrades from `unresolvable` to `compile failed` also leaves it, and
    # would otherwise read as a fix -- trading one failure for a worse one.
    disguised = sorted(s for s in left if reasons.get(s, "gone") is not None)
    fixed = sorted(s for s in left if reasons.get(s, "gone") is None)

    print(f"unresolved symbols: {len(cur)}  (baseline {len(base['unresolved'])})")
    print(f"eligible:           {eligible}  (baseline {base.get('eligible', 0)})")
    if fixed:
        print(f"  {len(fixed)} fixed since the baseline -- run --update to bank it")

    if disguised:
        print(f"\n  {len(disguised)} symbols left the unresolved set without becoming")
        print("  eligible -- they now fail for another reason, which is not a fix:")
        for s in disguised[:10]:
            print(f"      {s}  ->  {reasons.get(s, 'no longer a candidate')}")
    if not bad and lost <= 0 and not disguised:
        print("OK: no new unresolvable references")
        return 0

    print("\nFAIL: this change adds references that resolve to nothing.")
    print("They would pass the byte gate regardless, because match.py compares")
    print("relocated words as wildcards and never looks at what a call targets.\n")
    for sym, names, why in bad[:25]:
        print(f"  {why}: {sym}\n      {', '.join(names[:4])}")
    if len(bad) > 25:
        print(f"  ... and {len(bad) - 25} more")
    if lost > 0:
        print(f"\n  {lost} functions stopped being eligible -- a file that degrades from")
        print("  'unresolvable' to 'compile failed' leaves the unresolved set entirely,")
        print("  so the count is ratcheted too and cannot be traded away.")
    print("\nResolve them from the ROM's own relocation data:")
    print("  python tools/resolve_placeholders.py            # report")
    print("  python tools/resolve_placeholders.py --apply    # rewrite + verify")
    return 1


if __name__ == "__main__":
    sys.exit(main())
