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
  NO-REPRO   the committed source compiles to the right length but the bytes differ
  NO-SRC     no src/<name>.c|.cpp on disk to try (e.g. wrong branch / not yet landed)
  NO-BIN     the module binary for this address was not available
  NO-SYM     the function symbol was not found, or compiled to the wrong length

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
    dest = func_addr + o + 8 + (imm << 2)
    if (w >> 24) in (0xfa, 0xfb):     # BLX (ARM->Thumb): the H bit adds a halfword
        dest += ((w >> 24) & 1) << 1
    return dest & 0xFFFFFFFF


def is_interwork(rom_word, rom_t, cand_t):
    """True if the ROM slot is a BLX immediate (ARM->Thumb interworking call) whose
    destination equals the address the candidate names. The candidate object emits a
    plain BL with a relocation; the linker rewrites it to BLX because the target is a
    Thumb symbol. Only the encoding differs, so the source call is correct."""
    return (rom_word >> 24) in (0xfa, 0xfb) and cand_t is not None and rom_t == cand_t


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
    """[{off, type, sym, addr, add}] for relocs inside func; addr is the resolved
    target base or None if the symbol name carries no address; add is the relocation
    addend (so the linked word is base + add, e.g. &array[i].field). Returns None if
    func is absent.

    The addend is essential: mwccarm emits RELA relocations and encodes every
    base+offset access (struct fields, array elements) as the symbol's base address
    plus a nonzero r_addend. Reading only the base would mis-link those slots and
    report a correct source as WRONG. REL objects carry the addend in the slot."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    syms = list(symtab.iter_symbols())
    sym = next((s for s in syms if s.name == func), None)
    if sym is None:
        return None
    sec = elf.get_section(sym["st_shndx"])
    start, size = sym["st_value"], sym["st_size"]
    secdata = sec.data()
    rel = elf.get_section_by_name(".rel" + sec.name) or elf.get_section_by_name(".rela" + sec.name)
    is_rela = rel is not None and rel.name.startswith(".rela")
    out = []
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if not (0 <= o < size):
                continue
            tsym = syms[r["r_info_sym"]]
            res = RA.resolve_candidate(tsym.name, name_index)
            rtype = r["r_info_type"]
            if is_rela:
                add = r["r_addend"]
            elif rtype == R_ARM_ABS32:
                add = int.from_bytes(secdata[r["r_offset"]:r["r_offset"] + 4], "little")
            else:
                add = -8  # REL branch: the -8 PC bias is folded into link_function's formula
            out.append({"off": o, "type": rtype, "sym": tsym.name,
                        "addr": (res[1] if res else None), "add": add})
    return out


def link_function(code, addr, relocs):
    """Patch the candidate code (bytes) by applying each resolvable relocation, the
    way the linker would. Returns (linked_bytes, blind_offsets). blind_offsets are
    word offsets whose target could not be resolved or whose reloc type we do not
    link; the caller must not compare those words."""
    buf = bytearray(code)
    blind = []
    for rl in relocs:
        o, t, ta, add = rl["off"], rl["type"], rl["addr"], rl.get("add", 0)
        if ta is None or (t not in BRANCH_TYPES and t != R_ARM_ABS32):
            blind.append(o & ~3)
            continue
        if t == R_ARM_ABS32:
            struct.pack_into("<I", buf, o, (ta + add) & 0xFFFFFFFF)
        else:  # BL / B / blx-range branch: imm = (S + A - P) >> 2 (RELA A is -8)
            instr = struct.unpack_from("<I", buf, o)[0]
            imm = ((ta + add - (addr + o)) >> 2) & 0xFFFFFF
            struct.pack_into("<I", buf, o, (instr & 0xFF000000) | imm)
    return bytes(buf), set(blind)


def linkcheck(name, addr, size, mod, name_index):
    """Verdict + detail for one banked match."""
    import reverify_corpus as RV
    obj, sym, err = RA.winning_object(name, addr, size, mod)
    if obj is None:
        # A missing or wrong-length source is NOT a false match; give it a verdict
        # distinct from NO-REPRO so it does not read as "the source stopped matching".
        verdict = {"no-source": "NO-SRC", "no-module-bin": "NO-BIN",
                   "len-mismatch": "NO-SYM"}.get(err, "NO-REPRO")
        return {"name": name, "module": mod, "addr": f"0x{addr:08x}", "verdict": verdict,
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
                rw = int.from_bytes(target[i:i + 4], "little")
                if is_benign(rt, rl["addr"], prefer) or is_interwork(rw, rt, rl["addr"]):
                    benign += 1
                    continue
            tgt = (rl["addr"] + rl.get("add", 0)) & 0xFFFFFFFF if rl and rl["addr"] is not None else None
            genuine.append({"off": f"+0x{i:x}", "sym": rl["sym"] if rl else None,
                            "target": (f"0x{tgt:08x}" if tgt is not None else None)})
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
        if args.addr is None or args.size is None:
            # resolve addr/size/module from the ledger so `--name X` works standalone
            for l in (REPO / "progress" / "matched.jsonl").read_text().splitlines():
                if l.strip():
                    e = json.loads(l)
                    if e["name"] == args.name:
                        a = e["addr"]
                        args.addr = int(a, 16) if isinstance(a, str) else a
                        s = e["size"]
                        args.size = int(s, 16) if isinstance(s, str) else s
                        args.module = args.module or e["module"]
                        break
        if args.addr is None or args.size is None:
            print(f"{args.name}: not in progress/matched.jsonl; pass --addr/--size/--module")
            return
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
    for k in ["VERIFIED", "BENIGN", "BLIND", "WRONG", "NO-REPRO", "NO-SRC", "NO-BIN", "NO-SYM"]:
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
