"""Match a single function: compile candidate C with mwccarm and compare the
result to the function's bytes in the ROM, relocation-aware.

This is the core decomp loop in miniature. Almost every real function pools an
absolute address (a global, a callee), which the compiler emits as a relocation
slot of zeroes plus a reloc entry, while the ROM has the final linked address
there. So a correct match is: every instruction word identical AND every reloc
slot lined up -- not a raw byte-for-byte compare.

Usage:
    python tools/match.py --c match/f.c --func f --addr 0x02065a84 --size 0x10 \
        --version 1.2/sp2p3 --flags "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e"

Without --version, sweeps a default list of mwccarm versions and reports the best.
"""
import argparse
import pathlib
import subprocess
import tempfile
import os

from elftools.elf.elffile import ELFFile
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

REPO = pathlib.Path(__file__).resolve().parent.parent
MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
ARM9 = REPO / "extracted" / "arm9_dec.bin"
ARM9_BASE = 0x02004000

DEFAULT_FLAGS = "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc"
SWEEP = ["1.2/base", "1.2/sp2", "1.2/sp2p3", "1.2/sp3", "1.2/sp4",
         "2.0/base", "2.0/sp1", "2.0/sp1p2", "2.0/sp2", "2.0/sp2p2", "2.0/sp2p3"]
# The CodeWarrior 1.2 trio that survived version-pinning.
PINNED = ["1.2/base", "1.2/sp2", "1.2/sp2p3"]
# Proven (45 probe constructs + 60 ROM functions) byte-identical across the trio,
# so one is the canonical compiler and a single compile is a sufficient check.
CANONICAL = "1.2/sp2p3"

md = Cs(CS_ARCH_ARM, CS_MODE_ARM)


def target_bytes(addr: int, size: int, bin_path: pathlib.Path = ARM9, base: int = ARM9_BASE) -> bytes:
    data = bin_path.read_bytes()
    off = addr - base
    return data[off:off + size]


def compile_c(cfile: pathlib.Path, version: str, flags: str) -> bytes | None:
    """Compile C -> object with the given mwccarm version. Returns object bytes."""
    exe = MW / version / "mwccarm.exe"
    if not exe.is_file():
        print(f"  ! no compiler at {version}")
        return None
    with tempfile.TemporaryDirectory() as td:
        out_o = pathlib.Path(td) / "out.o"
        env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE))
        cmd = [str(exe), *flags.split(), "-c", str(cfile), "-o", str(out_o)]
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, env=env, timeout=90)
        except subprocess.TimeoutExpired:
            print(f"  ! compile timed out ({version})")
            return None
        if r.returncode != 0 or not out_o.is_file():
            print(f"  ! compile failed ({version}): {r.stderr.strip()[:200]}")
            return None
        return out_o.read_bytes()


def extract_func(obj: bytes, func: str):
    """Return (code_bytes, set_of_reloc_word_offsets) for `func` in the object."""
    import io
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    sym = next((s for s in symtab.iter_symbols() if s.name == func), None)
    if sym is None:
        return None, None
    sec = elf.get_section(sym["st_shndx"])
    start, size = sym["st_value"], sym["st_size"]
    code = sec.data()[start:start + size]
    relocs = set()
    rel = elf.get_section_by_name(".rel" + sec.name) or elf.get_section_by_name(".rela" + sec.name)
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if 0 <= o < size:
                relocs.add(o & ~3)
    return code, relocs


def compare(target: bytes, cand: bytes, relocs: set, verbose: bool = True):
    """Word-by-word compare; reloc slots are wildcards. Returns (ok, n_mismatch)."""
    if len(target) != len(cand):
        if verbose:
            print(f"  size differs: target 0x{len(target):x} vs candidate 0x{len(cand):x}")
        return False, 999
    ok = True
    ndiff = 0
    for i in range(0, len(target), 4):
        tw = target[i:i + 4]
        cw = cand[i:i + 4]
        if i in relocs:
            tag = "reloc (wildcard)"
            match = True
        else:
            match = tw == cw
            tag = "OK" if match else "<<< MISMATCH"
        if not match:
            ok = False
            ndiff += 1
        if verbose:
            ti = next(md.disasm(tw, 0), None)
            ci = next(md.disasm(cw, 0), None)
            ts = f"{ti.mnemonic} {ti.op_str}" if ti else tw.hex()
            cs = f"{ci.mnemonic} {ci.op_str}" if ci else cw.hex()
            print(f"   +0x{i:02x} | {tw.hex()} {ts:24} | {cw.hex()} {cs:24} | {tag}")
    return ok, ndiff


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--c", required=True)
    ap.add_argument("--func", required=True)
    ap.add_argument("--addr", required=True, type=lambda x: int(x, 0))
    ap.add_argument("--size", required=True, type=lambda x: int(x, 0))
    ap.add_argument("--version", default=None, help="check a single version")
    ap.add_argument("--versions", default=None, help="comma list to sweep")
    ap.add_argument("--trio", action="store_true", help="sweep the 1.2 base/sp2/sp2p3 trio")
    ap.add_argument("--all", action="store_true", help="sweep every known version")
    ap.add_argument("--brief", action="store_true", help="terse: per-version pass/fail; diff only if none match")
    ap.add_argument("--flags", default=DEFAULT_FLAGS)
    ap.add_argument("--bin", default=None,
                    help="override target binary (e.g. an overlay) instead of arm9_dec.bin")
    ap.add_argument("--base", default=None, type=lambda x: int(x, 0),
                    help="load address of --bin (required with --bin)")
    ap.add_argument("--strict-relocs", action="store_true",
                    help="beyond the byte compare, verify each reloc slot points at the "
                         "destination config/<module>/relocs.txt records (catches wrong "
                         "callee/global the wildcard would otherwise hide)")
    ap.add_argument("--module", default="arm9",
                    help="module name for --strict-relocs config lookup (arm9, ov006, ...)")
    args = ap.parse_args()

    strict = None
    if args.strict_relocs:
        import reloc_audit as RA
        import relocs as RL
        strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())

    cfile = pathlib.Path(args.c)
    if args.bin:
        tgt = target_bytes(args.addr, args.size, pathlib.Path(args.bin), args.base)
    else:
        tgt = target_bytes(args.addr, args.size)
    print(f"TARGET {args.func} @ 0x{args.addr:08x} size 0x{args.size:x}  bytes: {tgt.hex()}")

    if args.version:
        versions = [args.version]
    elif args.versions:
        versions = [v.strip() for v in args.versions.split(",") if v.strip()]
    elif args.all:
        versions = SWEEP
    elif args.trio:
        versions = PINNED          # the 1.2 trio (they're codegen-identical)
    else:
        versions = [CANONICAL]     # default: single canonical compile (3x cheaper)

    matched = []
    closest = None  # (ndiff, version, code, relocs) for a helpful diff when nothing matches
    for v in versions:
        obj = compile_c(cfile, v, args.flags)
        if obj is None:
            continue
        code, relocs = extract_func(obj, args.func)
        if code is None:
            print(f"  {v}: symbol '{args.func}' not found in object")
            continue
        if not args.brief:
            print(f"\n=== mwccarm {v} ===")
        ok, ndiff = compare(tgt, code, relocs, verbose=not args.brief)
        if ok and strict is not None:
            RA, name_index, config_relocs, sym_index = strict
            rows, missing = RA.check_destinations(obj, args.func, args.addr, args.size,
                                                  args.module, name_index, config_relocs, sym_index)
            bad = [r for r in (rows or []) if r["verdict"] == "WRONG-DEST"]
            if bad:
                ok = False
                print(f"  {v}: bytes match but {len(bad)} reloc destination(s) WRONG -- "
                      f"not a real match:")
                for r in bad:
                    print(f"      {r['off']:6} cand {r['cand']} ({r['cand_addr']}) "
                          f"!= config {r['cfg']}")
        if ok:
            matched.append(v)
            if args.brief:
                print(f"  {v}: MATCH")
        else:
            if args.brief:
                print(f"  {v}: {ndiff} word(s) differ")
            if closest is None or ndiff < closest[0]:
                closest = (ndiff, v, code, relocs)

    if args.brief and not matched and closest is not None:
        print(f"\n--- closest: {closest[1]} ({closest[0]} differ) ---")
        compare(tgt, closest[2], closest[3], verbose=True)

    print("\n" + "=" * 40)
    print("MATCHING VERSIONS:", ", ".join(matched) if matched else "none")


if __name__ == "__main__":
    main()
