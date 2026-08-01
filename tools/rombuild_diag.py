"""Explain why a source-built function does not reproduce the ROM.

`rombuild_check.py` names the functions that differ; this says what is different about
them. For every differing word it decodes both sides and, where the word is a branch or
a pointer, resolves the address each side names back to a symbol. That turns a byte diff
into a sentence: "the source calls X, the ROM calls Y".

This is the wrong-relocation-destination class - the one the match gate cannot see,
because `match.py` wildcards every word the candidate relocates, so a call to the wrong
function with the right shape still reads as a match.

Usage:
    python tools/rombuild_diag.py                    # diagnose every mismatch
    python tools/rombuild_diag.py --name func_...    # just one
    python tools/rombuild_diag.py --summary          # counts by shape only
"""
import argparse
import collections
import pathlib
import re
import sys

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from enroll import read_delinks, sections, CONFIG, REPO  # noqa: E402
from rombuild_check import complete_entries, module_binaries  # noqa: E402

SYM = re.compile(r"^(\S+)\s+kind:(\S+?)(?:\(([^)]*)\))?\s+addr:0x([0-9a-fA-F]+)")
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)


SPANS = []   # sorted [(start, end, name, module)] - filled by symbol_index()


def symbol_index():
    """address -> [(name, module)], plus a sorted span table for base+addend lookups."""
    idx = collections.defaultdict(list)
    spans = []
    for sp in sorted(CONFIG.rglob("symbols.txt")):
        label = sp.parent.relative_to(CONFIG).as_posix()
        for line in sp.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYM.match(line)
            if not m:
                continue
            addr = int(m.group(4), 16)
            idx[addr].append((m.group(1), label))
            size = 0
            if m.group(3):
                sm = re.search(r"size=0x([0-9a-fA-F]+)", m.group(3))
                if sm:
                    size = int(sm.group(1), 16)
            spans.append((addr, addr + max(size, 1), m.group(1), label))
    spans.sort()
    SPANS[:] = spans
    return idx


def name_at(idx, addr, prefer, allow_offset=True):
    """Resolve an address to a symbol, or to `symbol+0xN` for a base+addend pointer.

    mwccarm emits every struct-field / array-element access as the symbol's base
    address plus a nonzero addend, so an exact-address lookup misses most data
    pointers. Falling back to the containing symbol is what makes a wrong *base*
    distinguishable from a wrong *offset*.
    """
    hits = idx.get(addr)
    if hits:
        for (n, mod) in hits:
            if mod == prefer:
                return n
        return hits[0][0] + (f" [{hits[0][1]}]" if len(hits) == 1 else " [ambiguous]")
    if not allow_offset or not (0x01FF0000 <= addr < 0x02400000):
        return None
    import bisect
    i = bisect.bisect_right(SPANS, (addr, float("inf"), "", "")) - 1
    # walk back over shorter spans that start at the same place
    while i >= 0:
        start, end, n, mod = SPANS[i]
        if start <= addr < end:
            return f"{n}+0x{addr - start:x}" + ("" if mod == prefer else f" [{mod}]")
        if start < addr - 0x4000:
            break
        i -= 1
    return None


def branch_target(word, pc):
    """Decode an ARM B/BL/BLX at `pc`; return (mnemonic, target) or None."""
    cond = (word >> 28) & 0xF
    op = (word >> 24) & 0xF
    if cond != 0xF and op in (0xA, 0xB):
        off = word & 0xFFFFFF
        if off & 0x800000:
            off -= 0x1000000
        return ("bl" if op == 0xB else "b"), (pc + 8 + off * 4) & 0xFFFFFFFF
    if cond == 0xF and (word >> 25) & 0x7 == 0x5:      # BLX (immediate)
        off = word & 0xFFFFFF
        if off & 0x800000:
            off -= 0x1000000
        h = (word >> 24) & 1
        return "blx", (pc + 8 + off * 4 + h * 2) & 0xFFFFFFFF
    return None


def dis(word, pc):
    b = word.to_bytes(4, "little")
    for i in md.disasm(b, pc):
        return f"{i.mnemonic} {i.op_str}".strip()
    return f".word 0x{word:08x}"


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--name")
    ap.add_argument("--summary", action="store_true")
    ap.add_argument("--limit", type=int, default=0)
    args = ap.parse_args()

    idx = symbol_index()
    shapes = collections.Counter()
    repoint = collections.Counter()
    shown = 0

    for sp in sorted(CONFIG.rglob("symbols.txt")):
        d = sp.parent
        dl = d / "delinks.txt"
        if not dl.is_file():
            continue
        entries = complete_entries(dl)
        if not entries:
            continue
        built_p, retail_p = module_binaries(d)
        if not built_p or not built_p.is_file() or not retail_p.is_file():
            continue
        header, _ = read_delinks(dl)
        secs = sections(header)
        if not secs:
            continue
        base = min(s[1] for s in secs)
        built, retail = built_p.read_bytes(), retail_p.read_bytes()
        label = d.relative_to(CONFIG).as_posix()

        for (rel, addr, end) in entries:
            name = pathlib.Path(rel).stem
            if args.name and name != args.name:
                continue
            lo, hi = addr - base, end - base
            if hi > len(retail) or hi > len(built) or built[lo:hi] == retail[lo:hi]:
                continue
            lines = []
            for off in range(0, hi - lo, 4):
                bw = int.from_bytes(built[lo + off:lo + off + 4], "little")
                rw = int.from_bytes(retail[lo + off:lo + off + 4], "little")
                if bw == rw:
                    continue
                pc = addr + off
                bt, rt = branch_target(bw, pc), branch_target(rw, pc)
                if bt and rt:
                    bn = name_at(idx, bt[1], label) or f"0x{bt[1]:08x}"
                    rn = name_at(idx, rt[1], label) or f"0x{rt[1]:08x}"
                    if bt[0] != rt[0]:
                        shapes[f"{bt[0]} vs {rt[0]} (interworking)"] += 1
                        lines.append(f"    +0x{off:03x}  {bt[0]} {bn}   ROM: {rt[0]} {rn}")
                    else:
                        shapes["branch to a different symbol"] += 1
                        repoint[f"{bn} -> {rn}"] += 1
                        lines.append(f"    +0x{off:03x}  calls {bn}   ROM calls {rn}")
                elif name_at(idx, bw, label) or name_at(idx, rw, label):
                    bn = name_at(idx, bw, label) or f"0x{bw:08x}"
                    rn = name_at(idx, rw, label) or f"0x{rw:08x}"
                    shapes["pointer word to a different symbol"] += 1
                    repoint[f"{bn} -> {rn}"] += 1
                    lines.append(f"    +0x{off:03x}  .word {bn}   ROM: {rn}")
                else:
                    shapes["plain instruction/data difference"] += 1
                    lines.append(f"    +0x{off:03x}  {dis(bw, pc)}   ROM: {dis(rw, pc)}")
            if not args.summary:
                print(f"{name}  ({label}, 0x{addr:08x}, {len(lines)} differing word(s))")
                for l in lines[:8]:
                    print(l)
                if len(lines) > 8:
                    print(f"    ... {len(lines) - 8} more")
                print()
            shown += 1
            if args.limit and shown >= args.limit:
                break

    print("=== differing words by shape ===")
    for k, v in shapes.most_common():
        print(f"{v:6d}  {k}")
    if repoint:
        print("\n=== most common repoints (source -> what the ROM actually uses) ===")
        for k, v in repoint.most_common(15):
            print(f"{v:6d}  {k}")


if __name__ == "__main__":
    main()
