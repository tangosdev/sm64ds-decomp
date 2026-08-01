"""Resolve invented symbol names to the address the ROM actually uses.

[`link-verification.md`](../notes/link-verification.md) calls these the BLIND matches: a
`src/` file references a name that carries no address — an invented placeholder like `G0`
or `VT1`, or a plausible-looking name that is simply not in `config/**/symbols.txt`. No
static check can verify where such a relocation points, and the ROM build cannot link the
file at all, because the name resolves to nothing.

But the destination is not actually unknown. The function byte-matches the ROM, so the
ROM's own linked word at that relocation slot *is* the answer: decode it (a `BL` target,
or an `R_ARM_ABS32` pool word minus its addend) and look the address up in `symbols.txt`.

This does that per file and per relocation, so it handles both shapes uniformly: a head
symbol shared by hundreds of files, and a per-file placeholder like `G0` that means a
different address in every file.

    unresolved   ->   the address the ROM links to   ->   the canonical symbol there

Where a canonical symbol exists, the reference is renamed in `src/`. Where the address has
no symbol at all, nothing is renamed and the address is reported — those need a name added
to `symbols.txt`, which is a config decision rather than a source fix.

Every edit is byte-verified against the ROM before it is kept.

Usage:
    python tools/resolve_blind.py                  # report only
    python tools/resolve_blind.py --apply
    python tools/resolve_blind.py --name G0        # just one unresolved symbol
"""
import argparse
import bisect
import collections
import concurrent.futures
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M              # noqa: E402
import modules as MOD          # noqa: E402
import reloc_audit as RA       # noqa: E402
import linkcheck as LC         # noqa: E402
from enroll import candidates, CONFIG, REPO  # noqa: E402
from rombuild import CFLAGS, VERSION, versions  # noqa: E402

SYM = re.compile(r"^(\S+)\s+kind:(\S+?)(?:\(([^)]*)\))?\s+addr:0x([0-9a-fA-F]+)")


def symbol_tables():
    """(exact address -> name, sorted spans) per module label, plus every known name."""
    exact, spans, names = {}, [], set()
    for sp in sorted(CONFIG.rglob("symbols.txt")):
        rel = sp.parent.relative_to(CONFIG).as_posix()
        label = "arm9" if rel == "arm9" else rel.split("/")[-1]
        for line in sp.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYM.match(line)
            if not m:
                continue
            addr, name = int(m.group(4), 16), m.group(1)
            names.add(name)
            exact.setdefault((label, addr), name)
            exact.setdefault((None, addr), name)
            size = 0
            if m.group(3):
                sm = re.search(r"size=0x([0-9a-fA-F]+)", m.group(3))
                if sm:
                    size = int(sm.group(1), 16)
            spans.append((addr, addr + max(size, 1), name, label))
    spans.sort()
    return exact, spans, names


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--name", help="only resolve this unresolved symbol")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    ap.add_argument("--show", type=int, default=20)
    args = ap.parse_args()

    exact, spans, known = symbol_tables()
    starts = [s[0] for s in spans]
    name_index = RA.build_name_index()
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}
    vers = versions()
    cands, _ = candidates()

    def label_of(d):
        rel = d.relative_to(CONFIG).as_posix()
        return "arm9" if rel == "arm9" else rel.split("/")[-1]

    def resolve_addr(addr, label):
        n = exact.get((label, addr)) or exact.get((None, addr))
        if n:
            return n, 0
        i = bisect.bisect_right(starts, addr) - 1
        while i >= 0 and starts[i] > addr - 0x10000:
            lo, hi, nm, lb = spans[i]
            if lo <= addr < hi:
                return nm, addr - lo
            i -= 1
        return None, None

    def work(c):
        d, name, rel, addr, size, sec = c
        label = label_of(d)
        src = REPO / rel
        flags = CFLAGS
        if src.read_text(encoding="utf-8", errors="ignore").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
        obj = M.compile_c(src, vers.get(name, VERSION), flags)
        if obj is None:
            return None
        try:
            relocs = LC.func_relocs_typed(obj, name, name_index)
        except Exception:
            return None
        if not relocs:
            return None
        blind = [r for r in relocs if r["addr"] is None and r["sym"] not in known]
        if not blind:
            return None
        if label == "arm9":
            code = M.target_bytes(addr, size)
        elif label in mods:
            code = M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"])
        else:
            return None
        found = []
        for r in blind:
            if args.name and r["sym"] != args.name:
                continue
            o = r["off"]
            if o + 4 > len(code):
                continue
            word = int.from_bytes(code[o:o + 4], "little")
            if r["type"] in LC.BRANCH_TYPES:
                off = word & 0xFFFFFF
                if off & 0x800000:
                    off -= 0x1000000
                dest = (addr + o + 8 + off * 4) & 0xFFFFFFFF
            elif r["type"] == LC.R_ARM_ABS32:
                dest = (word - r.get("add", 0)) & 0xFFFFFFFF
            else:
                continue
            canon, delta = resolve_addr(dest, label)
            found.append((r["sym"], dest, canon, delta))
        return (rel, name, label, found) if found else None

    results = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for r in ex.map(work, cands):
            if r:
                results.append(r)

    stats = collections.Counter()
    mapping = collections.defaultdict(collections.Counter)
    no_symbol = collections.Counter()
    renamable = []
    for (rel, name, label, found) in results:
        subs = {}
        for (sym, dest, canon, delta) in found:
            if canon is None:
                stats["address has no symbol"] += 1
                no_symbol[f"0x{dest:08x} [{label}]"] += 1
            elif delta == 0:
                stats["resolves to a canonical symbol"] += 1
                mapping[sym][canon] += 1
                subs[sym] = canon
            else:
                stats["resolves inside a symbol (base+offset)"] += 1
                mapping[sym][f"{canon}+0x{delta:x}"] += 1
        if subs:
            renamable.append((rel, name, label, subs))

    print(f"files with an unresolvable reference: {len(results)}")
    for k, v in stats.most_common():
        print(f"  {v:6d}  {k}")
    print(f"\nfiles where every unresolved name maps to a canonical symbol: {len(renamable)}")
    print(f"\ntop unresolved names and what the ROM says they are:")
    for sym, targets in sorted(mapping.items(), key=lambda kv: -sum(kv[1].values()))[:args.show]:
        tot = sum(targets.values())
        if len(targets) == 1:
            print(f"  {tot:5d}x  {sym}  ->  {next(iter(targets))}")
        else:
            print(f"  {tot:5d}x  {sym}  ->  {len(targets)} different targets "
                  f"(per-file placeholder): {', '.join(list(targets)[:3])}…")
    if no_symbol:
        print(f"\naddresses with no symbol at all ({sum(no_symbol.values())} refs, "
              f"{len(no_symbol)} addresses) - these need a symbols.txt entry:")
        for k, v in no_symbol.most_common(10):
            print(f"  {v:5d}x  {k}")

    if not args.apply:
        print("\n(report only - pass --apply to rename and verify)")
        return

    def apply_one(job):
        rel, name, label, subs = job
        f = REPO / rel
        original = f.read_text(encoding="utf-8", errors="ignore")
        new = original
        for old, canon in subs.items():
            new = re.sub(r"\b" + re.escape(old) + r"\b", canon, new)
        if new == original:
            return rel, "unchanged"
        f.write_text(new, encoding="utf-8", newline="\n")
        flags = CFLAGS
        if new.startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
        info = next(((a, s) for (d, n, r, a, s, _sec) in cands if r == rel), None)
        ok = False
        if info:
            addr, size = info
            tgt = (M.target_bytes(addr, size) if label == "arm9"
                   else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
            for v in [vers.get(name, VERSION)] + [x for x in M.SWEEP]:
                o = M.compile_c(f, v, flags)
                if o is None:
                    continue
                code, rl = M.extract_func(o, name)
                if code is None:
                    continue
                ok, _ = M.compare(tgt, code, rl, verbose=False)
                if ok:
                    break
        if not ok:
            f.write_text(original, encoding="utf-8", newline="\n")
            return rel, "reverted (stopped matching)"
        return rel, "renamed"

    outcome = collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, o in ex.map(apply_one, renamable):
            outcome[o] += 1
    print()
    for k, v in outcome.most_common():
        print(f"{v:6d}  {k}")


if __name__ == "__main__":
    main()
