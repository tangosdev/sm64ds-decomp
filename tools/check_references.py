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

The key is **(module, ROM address)**, never the symbol or file path. Both are mutable:
readability work renames `func_ov006_020e3578` to a real mangled method and source
files move between directories. The module is required because overlays reuse RAM
addresses. The eligibility report carries this identity directly; legacy baselines
keyed by symbol are resolved through the symbol tables from the same git revision.

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
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import eligible as E  # noqa: E402

BASELINE = REPO / "config" / "unresolved-baseline.json"
SYMBOL_RE = re.compile(r"^(\S+)\s+kind:function\([^)]*\).*\baddr:0x([0-9a-fA-F]+)")


def make_id(module, addr):
    addr = int(addr, 0) if isinstance(addr, str) else addr
    return f"{module}:0x{addr:08x}"


def _module_from_symbols_path(path):
    parts = pathlib.PurePosixPath(path).parts
    if parts[-1] != "symbols.txt" or "config" not in parts:
        return None
    rel = parts[parts.index("config") + 1:-1]
    if rel == ("arm9",):
        return "arm9"
    if len(rel) == 2 and rel[0] == "arm9":
        return rel[1]                 # itcm / dtcm
    if len(rel) == 3 and rel[:2] == ("arm9", "overlays"):
        return rel[2]                 # ovNNN
    return "/".join(rel) if rel else None


def symbol_identities(ref=None):
    """Unique function symbol -> module:address, optionally from a git revision."""
    if ref:
        listing = subprocess.run(
            ["git", "ls-tree", "-r", "--name-only", ref, "--", "config"],
            cwd=REPO, capture_output=True, text=True,
        )
        paths = [p for p in listing.stdout.splitlines() if p.endswith("/symbols.txt")]

        def read(path):
            out = subprocess.run(["git", "show", f"{ref}:{path}"], cwd=REPO,
                                 capture_output=True, text=True)
            return out.stdout if out.returncode == 0 else ""
    else:
        paths = [p.relative_to(REPO).as_posix()
                 for p in sorted((REPO / "config").rglob("symbols.txt"))]

        def read(path):
            return (REPO / path).read_text(encoding="utf-8", errors="ignore")

    found = {}
    for path in paths:
        module = _module_from_symbols_path(path)
        if not module:
            continue
        for line in read(path).splitlines():
            m = SYMBOL_RE.match(line)
            if m:
                found.setdefault(m.group(1), set()).add(make_id(module, int(m.group(2), 16)))
    return {name: next(iter(ids)) for name, ids in found.items() if len(ids) == 1}


def normalise_baseline(data, symbols):
    """Read both legacy name keys and the address-keyed baseline written today."""
    unresolved, names = {}, {}
    for key, value in data.get("unresolved", {}).items():
        if isinstance(value, list):
            name, missing = key, value
            identity = symbols.get(name, f"name:{name}")  # unresolved stays fail-closed
        else:
            identity = key
            name = value.get("name", key)
            missing = value.get("missing", [])
        unresolved[identity] = sorted(set(missing))
        names[identity] = name
    return {"eligible": data.get("eligible", 0), "unresolved": unresolved,
            "names": names}


def current(path, require_fresh=True, symbols=None):
    """Address-keyed unresolved/reason maps plus names and the eligible count."""
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
    symbols = symbols or symbol_identities()
    unresolved, eligible, reasons, names = {}, 0, {}, {}
    for r in entries:
        identity = (make_id(r["module"], r["addr"])
                    if r.get("module") and r.get("addr") else symbols.get(r["name"]))
        if identity is None:
            identity = f"name:{r['name']}"  # legacy report with no resolvable symbol
        reason = r.get("reason")
        reasons[identity] = reason
        names[identity] = r["name"]
        if reason is None:
            eligible += 1
        elif reason.startswith("unresolvable"):
            unresolved[identity] = sorted(set(r.get("missing") or []))
    return unresolved, eligible, reasons, names


def load_baseline(ref=None):
    if ref:
        out = subprocess.run(["git", "show", f"{ref}:{BASELINE.relative_to(REPO).as_posix()}"],
                             cwd=REPO, capture_output=True, text=True)
        raw = json.loads(out.stdout) if out.returncode == 0 else None
    else:
        raw = json.loads(BASELINE.read_text(encoding="utf-8")) if BASELINE.is_file() else None
    return normalise_baseline(raw, symbol_identities(ref)) if raw is not None else None


def write_baseline(unresolved, eligible, names):
    BASELINE.parent.mkdir(parents=True, exist_ok=True)
    rows = {identity: {"name": names.get(identity, identity), "missing": missing}
            for identity, missing in sorted(unresolved.items())}
    BASELINE.write_text(json.dumps(
        {"eligible": eligible, "unresolved": rows},
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


def changes(cur, reasons, base):
    """(bad references, disguised regressions, genuine fixes), all address-keyed."""
    bad = worse(cur, base["unresolved"])
    left = set(base["unresolved"]) - set(cur)
    disguised = sorted(identity for identity in left
                        if reasons.get(identity, "gone") is not None)
    fixed = sorted(identity for identity in left
                   if reasons.get(identity, "gone") is None)
    return bad, disguised, fixed


def display_name(identity, current_names, base_names):
    return current_names.get(identity) or base_names.get(identity) or identity


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

    cur, eligible, reasons, names = current(
        args.report, require_fresh=not args.allow_stale)
    base = load_baseline(args.against)

    if args.update:
        if base is not None:
            bad, regressed, _fixed = changes(cur, reasons, base)
            if regressed:
                print("refusing to update: symbols left the backlog by failing differently")
                for identity in regressed[:10]:
                    label = display_name(identity, names, base["names"])
                    print(f"  {label} ({identity}) -> "
                          f"{reasons.get(identity, 'no longer a candidate')}")
                return 1
            if bad or eligible < base.get("eligible", 0):
                print("refusing to update: this would raise the baseline, not lower it")
                for identity, missing, why in bad[:10]:
                    label = display_name(identity, names, base["names"])
                    print(f"  {why}: {label} ({identity})  "
                          f"({', '.join(missing[:3])})")
                if eligible < base.get("eligible", 0):
                    print(f"  eligible fell {base['eligible']} -> {eligible}")
                return 1
        write_baseline(cur, eligible, names)
        print(f"baseline: {len(cur)} unresolved symbols, {eligible} eligible")
        return 0

    if base is None:
        sys.exit(f"no baseline at {BASELINE}\nCreate one with --update.")

    bad, disguised, fixed = changes(cur, reasons, base)
    lost = base.get("eligible", 0) - eligible

    print(f"unresolved symbols: {len(cur)}  (baseline {len(base['unresolved'])})")
    print(f"eligible:           {eligible}  (baseline {base.get('eligible', 0)})")
    if fixed:
        print(f"  {len(fixed)} fixed since the baseline -- run --update to bank it")

    if disguised:
        print(f"\n  {len(disguised)} symbols left the unresolved set without becoming")
        print("  eligible -- they now fail for another reason, which is not a fix:")
        for identity in disguised[:10]:
            label = display_name(identity, names, base["names"])
            print(f"      {label} ({identity})  ->  "
                  f"{reasons.get(identity, 'no longer a candidate')}")
    if not bad and lost <= 0 and not disguised:
        print("OK: no new unresolvable references")
        return 0

    print("\nFAIL: this change adds references that resolve to nothing.")
    print("They would pass the byte gate regardless, because match.py compares")
    print("relocated words as wildcards and never looks at what a call targets.\n")
    for identity, missing, why in bad[:25]:
        label = display_name(identity, names, base["names"])
        print(f"  {why}: {label} ({identity})\n      {', '.join(missing[:4])}")
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
