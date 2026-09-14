"""Match a single function: compile candidate C with mwccarm and compare the
result to the function's bytes in the ROM, relocation-aware.

This is the core decomp loop in miniature. Almost every real function pools an
absolute address (a global, a callee), which the compiler emits as a relocation
slot of zeroes plus a reloc entry, while the ROM has the final linked address
there. So a correct match is: every instruction word identical AND every reloc
slot lined up -- not a raw byte-for-byte compare.

Usage:
    python tools/match.py --c match/f.c --func f --addr 0x02065a84 --size 0x10 \
        --version 2004/b56 --flags "-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e"

The repository's ``include/`` directory is always on the compiler search path.
Use ``--include-dir`` for an additional candidate-specific header tree.

Without --version, compiles once with the canonical version (2004/b56). Use
``--all`` to sweep every known mwccarm version, or ``--trio`` for the 1.2 trio.
"""
import argparse
import pathlib
import subprocess
import tempfile
import os
import shutil

from elftools.elf.elffile import ELFFile
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

REPO = pathlib.Path(__file__).resolve().parent.parent
MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
ARM9 = REPO / "extracted" / "arm9_dec.bin"
ARM9_BASE = 0x02004000
INCLUDE = REPO / "include"

# -w illpragmas is a WARNING flag only (no codegen effect, verified: all banked matches still
# match with it on).
#
# What it does NOT do, corrected 2026-09-13 after the older reading below cost several sweeps:
# it does NOT warn on an unknown pragma NAME. `#pragma totally_invented_pragma on` compiles
# silently under -w illpragmas, -warn illpragmas, -w on, -warnings on and -w all alike. It
# fires on an ILLEGAL ARGUMENT to a pragma mwccarm already knows -- `#pragma opt_common_subs
# bogus_value` warns with the caret under the argument. Control that the diagnostic stream is
# not simply being swallowed: -w all warns "function has no prototype" in the same file.
#
# Inverting that reading gives a partial positive control where this project had asserted
# there is none -- but mind the direction. A warning PROVES the name exists. Silence proves
# NOTHING: `#pragma pack(bogus)`, `mark bogus_value` and `once bogus_value` are all silent and
# all certainly exist, so the probe has a real false-negative class. Read a silent result as
# "not shown to exist", never as "absent". What makes the positive direction trustworthy is
# that the warning is NAME-gated, not argument-shape-gated: an unknown name with an illegal
# argument (`totally_invented_pragma bogus_value`) is silent too.
#
# Shown to exist this way: opt_common_subs, opt_dead_code, opt_propagation, opt_lifetimes,
# opt_strength_reduction, opt_unroll_loops, opt_vectorize_loops, inline_max_size,
# optimize_for_size, global_optimizer, peephole. Not shown to exist: opt_nopeephole,
# opt_schedule, scheduling -- earlier sweeps recorded the first two as inert FINDINGS, which
# they cannot be either way.
#
# Existence is a front-end fact and says nothing about whether the ARM backend honours the
# pragma, so it does not by itself hand you a lever. Swept across the whole near-miss bank
# afterwards: peephole, global_optimizer, opt_unroll_loops and inline_max_size are byte-inert
# on every scorable row -- recognised names the backend ignores. The live set is
# opt_common_subs, opt_propagation, optimize_for_size, opt_lifetimes, opt_dead_code and
# opt_strength_reduction, and on every row where one of them moves code it moves it WORSE.
# Use the probe to stop a sweep wasting cells on a name, not as evidence a lever exists.
#
# Two further screens for when that one is inconclusive. mwccarm 1.2/sp2p3 and 2.0/sp2p3 DO
# warn on an unknown pragma name where 2004/b56 -- the canonical matching compiler, and the
# only silent one of the three -- does not, so a second build answers the same question. And
# do not try to enumerate the vocabulary out of mwccarm.exe's string table: run through this
# same probe it returns 1,442 "valid" names, including `wcscat` and `while`.
#
# Not to be confused with the -opt OPTION: `-opt nopeephole` and `-opt noschedule` are
# rejected option names, so a sweep recording either as "measured inert" measured nothing.
# mwccarm's accepted -opt list is off|none, on, all|full, [no]space, [no]speed, l[evel],
# [no]cse|[no]commonsubs, [no]deadcode, [no]deadstore, [no]lifetimes, [no]loop[invariants],
# [no]prop[agation], [no]strength, [no]dead, display|dump.
#
# compile_c surfaces the warning even on a successful compile. See notes/mwccarm-codegen.md 6as.
DEFAULT_FLAGS = ("-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e "
                 "-gccext,on -msgstyle gcc -w illpragmas")
# The C++ lane needs one flag the C lane does not: `-Cpp_exceptions off`, which
# `rombuild.CFLAGS` carries and DEFAULT_FLAGS does not. Without it mwccarm threads exception
# cleanup through any function holding an object with a destructor, and the function's own
# .text stops equalling the ROM's -- so a C++ candidate was being scored against a compile
# the ROM build never performs. It is inert for a source with no exception state, which is
# why this went unnoticed: measured per function on this tree, both flag sets, of the 6,200
# committed //cpp rows config resolves a source for, 6,198 reproduce identically under both,
# one reproduces ONLY with this flag (func_ov006_020ea914, whose "7-word floor" three lanes
# measured was this defect), and none reproduces only without it. The same rescore over the
# 17 //cpp near-miss rows that carry a stored target moves exactly that one row.
# Kept OUT of DEFAULT_FLAGS on purpose: nearmiss/eval_pin.json pins DEFAULT_FLAGS verbatim
# and tools/test_nearmiss_db.py::EvalPinGuardTests fails CI if it moves without a full
# `nearmiss_db.py reeval`, which is lane-owned. See tools/build_pin.py (which derives its
# flags from rombuild for the same reason), tools/swarm.py CPP_FLAGS, and
# notes/mwccarm-codegen.md 6co.
CPP_EXCEPTIONS_FLAG = "-Cpp_exceptions off"
# The builds --all sweeps. This was a hand-written list of 12 while 25 mwccarm.exe were
# installed, so `--all` ("sweep every known version") silently skipped 13 -- including
# 2.0/sp1p5, sp1p6, sp1p7 and sp2p4, service packs of a family it already swept. Every
# "a full version sweep found nothing" negative recorded against that list was really a
# 12-of-25 sweep, so those negatives are weaker than they read.
#
# Derived from disk instead, so adding a compiler to tools/mwccarm/ puts it in the sweep.
# _PREFERRED goes first because a sweep reports its matches in order; the rest follow
# sorted. Anything installed but absent from _PREFERRED is still swept -- that is the
# whole point.
#
# 2004/b56 leads _PREFERRED, not trails it. It is the canonical matching compiler (see
# notes/rom-build.md, notes 6ai), and because a sweep reports in order, putting it later
# makes an earlier version win ties and get recorded as "the" match. That misreporting is
# real: a 711-file reconciliation first read as "672 under 1.2/base" purely from iteration
# order, and became 699 under 2004/b56 once the canonical build was tried first.
_PREFERRED = [# Canonical matching compiler first:
              "2004/b56",
              "1.2/base", "1.2/sp2", "1.2/sp2p3", "1.2/sp3", "1.2/sp4",
              "2.0/base", "2.0/sp1", "2.0/sp1p2", "2.0/sp2", "2.0/sp2p2", "2.0/sp2p3"]


def installed_versions():
    """Every mwccarm build present under tools/mwccarm, as version strings."""
    if not MW.is_dir():
        return []
    return sorted(p.parent.relative_to(MW).as_posix() for p in MW.rglob("mwccarm.exe"))


def _sweep_list():
    found = installed_versions()
    return [v for v in _PREFERRED if v in found] + [v for v in found if v not in _PREFERRED]


SWEEP = _sweep_list()
# The CodeWarrior 1.2 trio that survived version-pinning (codegen-identical to each
# other). Used by --trio only.
PINNED = ["1.2/base", "1.2/sp2", "1.2/sp2p3"]
# Canonical single-compile default for matching. 2004 build 0056 reproduces more of
# this corpus than the 1.2 service packs (notes/rom-build.md). Ships mwccarm only —
# the ROM link still uses 1.2/sp2p3 mwldarm (LD_VERSION in tools/rombuild.py).
CANONICAL = "2004/b56"

md = Cs(CS_ARCH_ARM, CS_MODE_ARM)


def target_bytes(addr: int, size: int, bin_path: pathlib.Path = ARM9, base: int = ARM9_BASE) -> bytes:
    data = bin_path.read_bytes()
    off = addr - base
    return data[off:off + size]


def compile_c(cfile: pathlib.Path, version: str, flags: str,
              include_dirs=()) -> bytes | None:
    """Compile C -> object with the given mwccarm version. Returns object bytes."""
    exe = MW / version / "mwccarm.exe"
    if not exe.is_file():
        print(f"  ! no compiler at {version}")
        return None
    # Cross-run the Windows compiler under an emulator when asked: the PR-validator
    # sandbox runs this in a Linux container and sets MWCCARM_LAUNCHER=wine so the PE is
    # invoked via Wine. Unset on native Windows -> the exe is run directly, so this is a
    # no-op for every existing caller. (Wine was verified byte-transparent: the native and
    # container corpus link-checks are identical.) This lives here so the build box can run
    # stock repo tooling instead of a hand-patched fork of match.py.
    launcher = os.environ.get("MWCCARM_LAUNCHER", "").split()
    if not launcher and os.name != "nt" and shutil.which("wine"):
        # The variable is the worker's to set, and the worker sets it for rombuild.py and
        # pr_linkcheck.py but not for validate_merge.py, whose _compiled_code_reader lands
        # here whenever a PR touches symbols.txt (#2496: "Exec format error" on the PE).
        # A Windows executable cannot run natively on a non-Windows host, so with Wine on
        # PATH and no launcher named, Wine is the only thing that can be meant. Native
        # Windows is untouched: os.name == "nt" never enters this branch.
        launcher = ["wine"]
    with tempfile.TemporaryDirectory() as td:
        out_o = pathlib.Path(td) / "out.o"
        env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE))
        # Candidate-specific includes come first so an external workbench can be
        # verified against its own headers. The canonical repository include tree
        # is always available and is the normal path used by committed sources.
        search = [pathlib.Path(p).resolve() for p in include_dirs]
        canonical = INCLUDE.resolve()
        if canonical not in search:
            search.append(canonical)
        cmd = [*launcher, str(exe), *flags.split()]
        for inc in search:
            cmd.extend(["-i", str(inc)])
        cmd.extend(["-c", str(cfile), "-o", str(out_o)])
        try:
            r = subprocess.run(cmd, capture_output=True, text=True, env=env, timeout=90)
        except subprocess.TimeoutExpired:
            print(f"  ! compile timed out ({version})")
            return None
        if r.returncode != 0 or not out_o.is_file():
            detail = "\n".join(s for s in (r.stdout.strip(), r.stderr.strip()) if s)
            print(f"  ! compile failed ({version}): {detail[:500]}")
            return None
        # A SUCCESSFUL compile can still be telling you something about the lever being
        # measured -- but note what this does and does not catch. mwccarm accepts an unknown
        # pragma NAME silently, so a typo'd name still slips past here and gets banked as
        # "inert". What it catches is a KNOWN pragma handed an argument it rejects: real
        # pragma, not applied. See DEFAULT_FLAGS above for the probe that tests a name.
        for line in (r.stdout + r.stderr).splitlines():
            if "illegal #pragma" in line:
                print(f"  ! {line.strip()}  <- mwccarm DOES know this pragma and is "
                      f"rejecting the ARGUMENT, so it is not applied (see notes 6as)")
        return out_o.read_bytes()


def extract_func(obj: bytes, func: str):
    """Return (code_bytes, set_of_reloc_word_offsets) for `func` in the object."""
    import io
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    sym = next((s for s in symtab.iter_symbols()
                if s.name == func and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS")), None)
    if sym is None:
        return None, None
    sec = elf.get_section(sym["st_shndx"])
    start, size = sym["st_value"], sym["st_size"]
    code = sec.data()[start:start + size]
    relocs = set()
    # Match the reloc section by sh_info, never by name: mwccarm emits one section per function
    # and names them all ".text", so a name lookup returns some other function's relocations
    # whenever the TU defines more than one (e.g. a C++ dtor emits D0/D1/D2 + thunks). Wrong
    # wildcard set = a real match can read as a mismatch, or a mismatch can be wildcarded away.
    rel = next((s for s in elf.iter_sections()
                if s.header["sh_type"] in ("SHT_REL", "SHT_RELA")
                and s.header["sh_info"] == sym["st_shndx"]), None)
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - start
            if 0 <= o < size:
                relocs.add(o & ~3)
    return code, relocs


def sole_func_symbol(obj: bytes):
    """Name of the object's ONLY defined, sized STT_FUNC symbol -- or None when it
    defines zero or two-or-more.

    extract_func matches a symbol by exact string, which is the right default but has
    two known ways to miss a function the object plainly defines:
      * the stored name is a stale func_<addr> placeholder (or, symmetrically, the
        C++ symbol) while the source spells the other one -- a symbol import renamed
        one side and not the other;
      * the repo spells a substitution-COMPRESSED Itanium mangling and mwccarm emits
        the expanded form for the same signature (_ZN5Model27LoadCompressedTextureToVramEPcjS0_
        vs ...EPcjPc). No exact-string lookup can ever bridge that.
    Neither is a real absence, and both are unambiguous when the object holds exactly
    one function. Two or more and there is nothing to disambiguate with, so callers
    must keep treating the miss as a miss."""
    import io
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    if symtab is None:
        return None
    funcs = [s.name for s in symtab.iter_symbols()
             if s["st_info"]["type"] == "STT_FUNC" and s["st_size"]
             and s["st_shndx"] not in ("SHN_UNDEF", "SHN_ABS") and s.name]
    return funcs[0] if len(set(funcs)) == 1 else None


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


def resolve_cpp_flags(cfile, flags):
    """The flag set `cfile` must actually be compiled with, given the requested `flags`.

    Auto-detect C++ the same way fdiff/swarm do: a leading //cpp marker means compile with
    -lang c++ instead of the default -lang c99, so C++ candidates stop failing to compile
    (the file is already .cpp, so it compiles in place - no temp copy needed).

    `-Cpp_exceptions off` then rides along with C++ mode HOWEVER THAT MODE WAS REACHED,
    because the build compiles every C++ source with it and without it a source holding an
    object with a destructor gets exception cleanup the ROM's bytes do not have (see
    CPP_EXCEPTIONS_FLAG above). Until now the flag was appended only inside the c99 -> c++
    rewrite, so `--flags "... -lang c++ ..."` passed by hand -- the documented way to compile
    a candidate that is C++ without a //cpp marker -- skipped it silently and scored the
    function against bytes the build would never produce. Nothing in the output said so:
    the operator sees a near miss and goes looking for a source-level cause that is not there.
    Same family as tools/pr_linkcheck.py, which compiled with flags the build does not use.

    An explicit `-Cpp_exceptions` on either side is left alone, so a caller can still measure
    the difference on purpose.
    """
    try:
        is_cpp_source = cfile.read_text(encoding="utf-8").startswith("//cpp")
    except OSError:
        # a missing/unreadable candidate surfaces later at compile_c with a clearer error
        is_cpp_source = False
    if is_cpp_source and "-lang c99" in flags:
        flags = flags.replace("-lang c99", "-lang c++")
    if "-lang c++" in flags and "-Cpp_exceptions" not in flags:
        flags += " " + CPP_EXCEPTIONS_FLAG
    return flags


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
    ap.add_argument("--include-dir", action="append", default=[],
                    help="additional header search directory (repeatable; checked before repo include/)")
    ap.add_argument("--bin", default=None,
                    help="override target binary (e.g. an overlay) instead of arm9_dec.bin")
    ap.add_argument("--base", default=None, type=lambda x: int(x, 0),
                    help="load address of --bin (required with --bin)")
    ap.add_argument("--strict-relocs", dest="strict_relocs", action="store_true", default=True,
                    help="(default) also verify each reloc slot points at the destination "
                         "config/<module>/relocs.txt records, so a wrong callee/global the byte "
                         "wildcard would otherwise hide is reported as NOT a match")
    ap.add_argument("--no-strict-relocs", dest="strict_relocs", action="store_false",
                    help="skip the reloc-destination check (loose byte-only compare)")
    ap.add_argument("--module", default="arm9",
                    help="module name for --strict-relocs config lookup (arm9, ov006, ...)")
    ap.add_argument("--cpp-check", action="store_true",
                    help="lint C++ file header and symbol naming conventions")
    args = ap.parse_args()

    cfile = pathlib.Path(args.c)
    if args.cpp_check and cfile.is_file():
        text = cfile.read_text(encoding="utf-8", errors="ignore")
        if cfile.suffix == ".cpp" and not text.startswith("//cpp"):
            print(f"  [cpp-check] Warning: {cfile.name} is a .cpp file but lacks '//cpp' header on line 1")
        if text.startswith("//cpp") and cfile.suffix == ".c":
            print(f"  [cpp-check] Warning: {cfile.name} has '//cpp' header but extension is .c (should be .cpp)")
        if cfile.stem.startswith("_Z") and args.func and args.func != cfile.stem:
            print(f"  [cpp-check] Warning: function '{args.func}' does not match file symbol name '{cfile.stem}'")

    strict = None
    if args.strict_relocs:
        try:
            import reloc_audit as RA
            import relocs as RL
            strict = (RA, RA.build_name_index(), RA.build_config_relocs(), RL.load_all_syms())
        except Exception as e:
            # No config/reloc data on this machine -> degrade to byte-only, don't break matching.
            print(f"  (reloc-destination check unavailable: {e}; byte-only compare)")

    cfile = pathlib.Path(args.c)
    flags = resolve_cpp_flags(cfile, args.flags)
    if args.bin:
        tgt = target_bytes(args.addr, args.size, pathlib.Path(args.bin), args.base)
    elif args.module and args.module != "arm9":
        # Overlay target: resolve its binary + load address from the modules helper,
        # the same way fdiff does, so callers only need --module ovNNN (no --bin/--base).
        import modules as MOD
        found = None
        for mod in MOD.modules():
            label = "arm9" if mod["name"] == "main" else mod["name"]
            if label == args.module:
                found = mod
                break
        if not found:
            raise SystemExit(f"module {args.module} not found (pass --bin/--base for it)")
        tgt = target_bytes(args.addr, args.size, found["bin"], found["base"])
    else:
        tgt = target_bytes(args.addr, args.size)
    hexstr = tgt.hex()
    shown = hexstr if (not args.brief or len(hexstr) <= 128) else f"{hexstr[:128]}... ({len(tgt)} bytes)"
    print(f"TARGET {args.func} @ 0x{args.addr:08x} size 0x{args.size:x}  bytes: {shown}")

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
        obj = compile_c(cfile, v, flags, args.include_dir)
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
            try:
                rows, missing = RA.check_destinations(obj, args.func, args.addr, args.size,
                                                      args.module, name_index, config_relocs, sym_index)
            except Exception as e:
                # Also not a pass. This used to print "skipped" and leave ok True, so
                # any exception in the check silently produced a verified match -- the
                # same defect as the unknown-module path below, by a different route.
                rows, missing = None, f"reloc-destination check raised: {e}"
            # rows is None means the check could not run at all -- an unknown --module
            # spelling, or the symbol missing from the object. Both used to fall through
            # `rows or []` to an empty `bad` and report a clean strict-reloc pass having
            # verified nothing, which is worse than not offering the flag. Fail instead.
            if rows is None:
                ok = False
                print(f"  {v}: bytes match but the reloc-destination check could not run "
                      f"-- NOT a verified match: {missing}")
                bad = []
            else:
                bad = [r for r in rows if r["verdict"] == "WRONG-DEST"]
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
