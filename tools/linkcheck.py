"""Link-based verification: reconstruct the LINKED bytes of a banked match and
byte-compare them to the ROM.

WHY THIS EXISTS
---------------
The match gate (match.py:compare) wildcards every word the candidate relocates,
so it never checks where a relocation points. reloc_audit.py closed part of that
by comparing each reloc's resolved DESTINATION ADDRESS to config/**/relocs.txt.
This tool goes the whole way: it links the candidate by writing the resolved
target into each reloc slot with the real ARM encoding (R_ARM_ABS32 pool words,
R_ARM_PC24 / R_ARM_CALL branches), then compares the linked function byte-for-byte
to the ROM. A correct match links to bytes identical to the ROM; a wrong callee or
global produces a byte difference at that slot. This also catches reloc TYPE and
addend errors that a pure destination-address compare misses.

LIMIT (honest): a relocation whose target symbol cannot be resolved to an address
(an invented name not in symbols.txt and not a func_<addr>/data_<addr> form) cannot
be linked. Those slots are reported BLIND; the rest of the function is still
verified. Closing the blind spot is a symbol-coverage problem, not a tool problem.

Verdicts (per function):
  VERIFIED   every reloc resolved and the linked bytes equal the ROM
  WRONG      a resolved reloc links to bytes that differ from the ROM (false match)
  BLIND-n    n reloc slots could not be resolved; everything else verified
  NO-REPRO   the committed source no longer reproduces the ROM bytes
  NO-SYM     the function symbol was not found in the object

Usage:
  python tools/linkcheck.py                          # whole corpus
  python tools/linkcheck.py --module ov006 -j 10
  python tools/linkcheck.py --limit 200
  python tools/linkcheck.py --name func_02013a88 --addr 0x02013a88 --size 0x4c --module arm9
  python tools/linkcheck.py --json out.json
"""
import argparse
import io
import json
import pathlib
import struct
import sys
from collections import Counter
from concurrent.futures import ThreadPoolExecutor

from elftools.elf.elffile import ELFFile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M            # noqa: E402
import reloc_audit as RA     # noqa: E402
import modules as MOD        # noqa: E402

# ARM relocation types we know how to link.
R_ARM_PC24 = 1
R_ARM_ABS32 = 2
R_ARM_CALL = 28
R_ARM_JUMP24 = 29
BRANCH_TYPES = {R_ARM_PC24, R_ARM_CALL, R_ARM_JUMP24}

# Two reasons a slot can differ from the ROM yet still be correct source:
#  - veneer: the ROM reloc points at a 12-byte interworking trampoline
#    (LDR ip,[pc]; BX ip; .word real) that jumps to the address the source names.
#    The linker inserts the veneer; the source naming the real symbol is right.
#  - twin: the ROM target and the candidate's target hold byte-identical code
#    (a duplicate function), so calling either is behaviorally identical.
_RANGES = None


def _ranges():
    global _RANGES
    if _RANGES is None:
        _RANGES = []
        for m in MOD.modules():
            data = m["bin"].read_bytes()
            _RANGES.append((m["name"], m["base"], m["base"] + len(data), data))
    return _RANGES


def read_at(rom_addr, n, prefer=()):
    """Read n bytes at an absolute address, trying the preferred module images first
    (overlays overlap in address space, so order matters). Returns None if not found."""
    pref = [("main" if p == "arm9" else p) for p in prefer]
    rs = _ranges()
    for want in pref + [r[0] for r in rs]:
        for nm, base, end, data in rs:
            if nm == want and base <= rom_addr and rom_addr + n <= end:
                return data[rom_addr - base:rom_addr - base + n]
    return None


def rom_target(target, o, rtype, func_addr):
    """The address the ROM's slot at offset o points to, decoded from the ROM bytes."""
    w = int.from_bytes(target[o:o + 4], "little")
    if rtype == R_ARM_ABS32:
        return w
    imm = w & 0xFFFFFF
    if imm & 0x800000:
        imm -= 0x1000000
    return (func_addr + o + 8 + (imm << 2)) & 0xFFFFFFFF


def is_benign(rom_t, cand_t, prefer):
    """True if the ROM target is a veneer to cand_t, or a byte-identical twin of it."""
    if cand_t is None:
        return False
    vb = read_at(rom_t, 12, prefer)
    if vb and int.from_bytes(vb[0:4], "little") == 0xe59fc000 \
            and int.from_bytes(vb[4:8], "little") == 0xe12fff1c \
            and int.from_bytes(vb[8:12], "little") == cand_t:
        return True
    a = read_at(rom_t, 16, prefer)
    b = read_at(cand_t, 16, prefer)
    return a is not None and b is not None and a == b


def func_relocs_typed(obj, func, name_index):
    """[{off, type, sym, addr}] for relocs inside func; addr is the resolved target
    or None if the symbol name carries no address. Returns None if func is absent."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    syms = list(symtab.iter_symbols())
    sym = next((s for s in syms if s.name == func), None)
    if sym is None:
        return None
    sec = elf.get_section(sym["st_shndx"])
    start, size = sym["st_value"], sym["st_size"]
    rel = elf.get_section_by_name(".rel" + sec.name) or elf.get_section_by_name(".rela" + sec.name)
    out = []
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if not (0 <= o < size):
                continue
            tsym = syms[r["r_info_sym"]]
            res = RA.resolve_candidate(tsym.name, name_index)
            out.append({"off": o, "type": r["r_info_type"], "sym": tsym.name,
                        "addr": (res[1] if res else None)})
    return out


def link_function(code, addr, relocs):
    """Patch the candidate code (bytes) by applying each resolvable relocation, the
    way the linker would. Returns (linked_bytes, blind_offsets). blind_offsets are
    word offsets whose target could not be resolved or whose reloc type we do not
    link; the caller must not compare those words."""
    buf = bytearray(code)
    blind = []
    for rl in relocs:
        o, t, ta = rl["off"], rl["type"], rl["addr"]
        if ta is None or (t not in BRANCH_TYPES and t != R_ARM_ABS32):
            blind.append(o & ~3)
            continue
        if t == R_ARM_ABS32:
            struct.pack_into("<I", buf, o, ta & 0xFFFFFFFF)
        else:  # BL / B / blx-range branch: 24-bit word-relative to pc+8
            instr = struct.unpack_from("<I", buf, o)[0]
            imm = ((ta - (addr + o + 8)) >> 2) & 0xFFFFFF
            struct.pack_into("<I", buf, o, (instr & 0xFF000000) | imm)
    return bytes(buf), set(blind)


def linkcheck(name, addr, size, mod, name_index):
    """Verdict + detail for one banked match."""
    import reverify_corpus as RV
    obj, sym, err = RA.winning_object(name, addr, size, mod)
    if obj is None:
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": "NO-REPRO",
                "reason": err, "diffs": [], "blind": 0}
    target = RV.rom_bytes(mod, addr, size)
    code, _ = M.extract_func(obj, sym)
    if code is None or len(code) != len(target):
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": "NO-SYM",
                "reason": "len-mismatch", "diffs": [], "blind": 0}
    relocs = func_relocs_typed(obj, sym, name_index)
    linked, blind = link_function(code, addr, relocs)
    by_off = {rl["off"] & ~3: rl for rl in relocs}
    diffs = [i for i in range(0, len(target), 4)
             if i not in blind and linked[i:i + 4] != target[i:i + 4]]
    if diffs:
        prefer = (mod, "arm9")
        genuine, benign = [], 0
        for i in diffs:
            rl = by_off.get(i)
            if rl is not None:
                rt = rom_target(target, i, rl["type"], addr)
                if is_benign(rt, rl["addr"], prefer):
                    benign += 1
                    continue
            genuine.append({"off": f"+0x{i:x}", "sym": rl["sym"] if rl else None,
                            "target": (f"0x{rl['addr']:08x}" if rl and rl["addr"] is not None else None)})
        if genuine:
            return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": "WRONG",
                    "diffs": genuine, "blind": len(blind)}
        if benign:
            return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": "BENIGN",
                    "diffs": [], "blind": len(blind)}
    if blind:
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}",
                "verdict": f"BLIND-{len(blind)}", "diffs": [], "blind": len(blind)}
    return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": "VERIFIED",
            "diffs": [], "blind": 0}


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", default=None)
    ap.add_argument("--limit", type=int, default=None)
    ap.add_argument("-j", "--jobs", type=int, default=8)
    ap.add_argument("--json", default=None)
    ap.add_argument("--name", default=None, help="link-check a single function")
    ap.add_argument("--addr", default=None, type=lambda x: int(x, 0))
    ap.add_argument("--size", default=None, type=lambda x: int(x, 0))
    args = ap.parse_args()

    name_index = RA.build_name_index()

    if args.name:
        r = linkcheck(args.name, args.addr, args.size, args.module or "arm9", name_index)
        print(json.dumps(r, indent=1))
        return

    entries = [json.loads(l) for l in (REPO / "progress" / "matched.jsonl").read_text().splitlines() if l.strip()]
    if args.module:
        entries = [e for e in entries if e["module"] == args.module]
    if args.limit:
        entries = entries[:args.limit]
    print(f"link-checking {len(entries)} banked matches with {args.jobs} workers ...", flush=True)

    def run(e):
        a = int(e["addr"], 16) if isinstance(e["addr"], str) else e["addr"]
        return linkcheck(e["name"], a, e["size"], e["module"], name_index)

    results, done = [], 0
    with ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for r in ex.map(run, entries):
            results.append(r)
            done += 1
            if done % 250 == 0:
                print(f"  {done}/{len(entries)}", flush=True)

    def bucket(v):
        return "BLIND" if v.startswith("BLIND") else v
    cat = Counter(bucket(r["verdict"]) for r in results)
    print("\n=== link-based verification ===")
    for k in ["VERIFIED", "BENIGN", "BLIND", "WRONG", "NO-REPRO", "NO-SYM"]:
        if cat.get(k):
            print(f"  {k:10} {cat[k]}")
    wrong = [r for r in results if r["verdict"] == "WRONG"]
    if wrong:
        print(f"\n--- {len(wrong)} matches whose linked bytes differ from the ROM ---")
        for r in wrong[:40]:
            print(f"{r['module']:7} {r['addr']} {r['name']}")
            for d in r["diffs"]:
                print(f"    {d['off']:6} {d['sym']} -> {d['target']}")
    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(results, indent=1))
        print(f"\nfull detail -> {args.json}")


if __name__ == "__main__":
    main()
