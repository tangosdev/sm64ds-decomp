"""Match a single function: compile candidate C with mwccarm and compare the
result to the function's bytes in the ROM, relocation-aware.

This is the core decomp loop in miniature. Almost every real function pools an
absolute address (a global, a callee), which the compiler emits as a relocation
slot of zeroes plus a reloc entry, while the ROM has the final linked address
there. So a correct match is: every instruction word identical AND every reloc
slot lined up -- not a raw byte-for-byte compare.

Usage:
    python tools/match.py --c match/f.c --func f --addr 0x02065a84 --size 0x10 \
        --version 2004/b56

Flags default to the ones the ROM build will actually hand this file -- `build_flags()`,
which resolves through `build_pin.flags_for`. `--flags` overrides them for a deliberate
lever sweep. Do NOT reach for `DEFAULT_FLAGS`: it is the sweep's string, it omits
`-Cpp_exceptions off`, and compiling this gate with it has already banked a byte-exact
C++ body as a divergence-7 near-miss.

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

from elftools.elf.elffile import ELFFile
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

REPO = pathlib.Path(__file__).resolve().parent.parent
MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
ARM9 = REPO / "extracted" / "arm9_dec.bin"
ARM9_BASE = 0x02004000
INCLUDE = REPO / "include"

# THE SWEEP'S flag set -- NOT the ROM build's. The build compiles with
# `-Cpp_exceptions off` (rombuild.CFLAGS) and this string does not; see build_flags()
# below for which question each one answers and why the CLI no longer defaults here.
# Kept as a single module-level literal on purpose: tools/test_nearmiss_db.py parses it
# out of this file with ast.literal_eval and fails if it is ever derived or reassigned.
#
# -w illpragmas DOES NOT DO WHAT THIS COMMENT USED TO CLAIM. The old text said mwccarm
# silently accepts an unknown #pragma by default and that this flag makes it warn, so a
# sweep could tell "inert lever" from "pragma does not exist". Measured by direct
# invocation, mwccarm 2004/b56, 2026-09-13:
#
#     #pragma totally_invented_pragma on     -w illpragmas -> NO diagnostic, exit 0
#                                            -w all        -> NO diagnostic, exit 0
#     #pragma opt_common_subs bogus_value    -w illpragmas -> "Warning: illegal #pragma",
#                                                             caret under the ARGUMENT
#
# So the flag is real and it is not a no-op -- it just fires on an illegal ARGUMENT to a
# pragma mwccarm KNOWS, never on an unknown pragma NAME. The `-w all` run is the positive
# control both ways: it warns about `function has no prototype` in the same file, so the
# silence on the invented name is the compiler's answer and not a swallowed stream.
# An "inert" reading of a pragma is therefore still indistinguishable from that pragma not
# existing, and HASH-SCREENING the object is the only valid screen for a pragma sweep.
#
# THERE IS, HOWEVER, A POSITIVE CONTROL FOR PRAGMA EXISTENCE -- use it before sweeping a
# name. Feed the pragma a deliberately illegal argument and compile with -w illpragmas:
#
#     printf '#pragma %s __bogus_arg__\nint q(void){return 0;}\n' NAME > probe.c
#     mwccarm -w illpragmas -c -o probe.o probe.c   # "illegal #pragma" => NAME EXISTS
#
# The argument has to be ILLEGAL for the probe to say anything: a known pragma given a
# LEGAL argument is silent, exactly like an unknown one. That is the trap that makes the
# naive reading of this flag useless and the probe useful.
#
# Measured over a mixed set: opt_common_subs, opt_dead_code, opt_propagation,
# opt_lifetimes, opt_strength_reduction, opt_unroll_loops, opt_vectorize_loops,
# inline_max_size, optimize_for_size, global_optimizer and `peephole` all warn (they
# exist); totally_invented_pragma, zzz_not_a_pragma, opt_nopeephole, opt_schedule and
# `scheduling` are all silent (they do not). `#pragma peephole off` then compiles clean.
# Independently reproduced the same day by a second lane under both `-lang c99` and
# `-lang c++`.
#
# WHAT DOES NOT WORK, so nobody spends the afternoon on it again: recovering the pragma
# vocabulary by feeding every string in `mwccarm.exe`'s string table through this flag.
# Tried; it returns 1,442 "valid" names, among them `wcscat` and `while`. The string
# table is not the pragma table, and the flag cannot tell you which strings in it are.
#
# There is a SECOND, independent screen in notes/mwccarm-codegen.md 6br: the 1.2, 2.0 and
# dsi builds DO warn on an unknown pragma name, so compiling the same file once under
# `--version 1.2/sp2p3` names a typo that 2004/b56 swallows. Use either; the two agreed
# on every name above. Only b56 -- the canonical matching compiler, so the one a sweep
# reaches for by default -- is silent, which is what made this trap expensive.
#
# AND THE -opt OPTION NAMES ARE NOT FREE-FORM EITHER. `-opt nopeephole` and
# `-opt noschedule` are REJECTED ("Unknown option ... while parsing option '-opt'"), so
# any sweep that recorded either as "measured inert" measured nothing at all. mwccarm
# 2004/b56 prints its own accepted list, verbatim:
#     off|none, on, all|full, [no]space, [no]speed, l[evel], [no]cse | [no]commonsubs,
#     [no]deadcode, [no]deadstore, [no]lifetimes, [no]loop[invariants],
#     [no]prop[agation], [no]strength, [no]dead, or display|dump
# mwccarm prints the same shape of list for a bad `-w` name, which is how the `-w`
# vocabulary above was established. Neither list is guessable; ask the compiler.
DEFAULT_FLAGS = ("-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e "
                 "-gccext,on -msgstyle gcc -w illpragmas")
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


def build_flags(path, text=None):
    """The flags the ROM build will hand mwccarm for this file. The CLI's default.

    Resolved through `build_pin.flags_for`, which is the ONE definition of the build's
    flag string (it reads `rombuild.CFLAGS` and swaps `-lang c99` for `-lang c++` on the
    `//cpp` first-line marker, exactly as `rombuild.compile_one` does). Deliberately a
    reference and not a second copy: PR #2659 moved `linkcheck.py`, `reloc_audit.py` and
    `bytegate.py`'s link gate here for the same reason, and a hand-synced duplicate is
    the thing that goes stale.

    WHY THIS IS NOT `DEFAULT_FLAGS`. The two strings differ by `-Cpp_exceptions off`, and
    for a C++ body that flag can move `.text`. Measured 2026-09-13: `func_ov006_020ea914`
    (ov006, 0x324 bytes) sat in the near-miss bank at divergence 7 for months. The
    divergence was entirely the missing flag -- seven instructions reordered at
    +0x98..+0xbc -- and the already-banked draft is byte-exact with no source change at
    all when compiled the way the build compiles it. Isolated: adding only
    `-Cpp_exceptions off` to `DEFAULT_FLAGS` takes it 7 -> 0, and removing only
    `-w illpragmas` leaves the `.text` hash unchanged.

    That is a false NEAR-MISS. The same gap runs the other way -- a body that reproduces
    only with exceptions ON would be reported here as a match the ROM build then cannot
    link -- which is why the answer is the build's flags rather than "both".

    `-w illpragmas` is dropped by this rule rather than added to the build's string, and
    that is a deliberate choice, not an oversight. It is codegen-inert (verified: the
    `.text` hash is identical with and without it on every C++ row in the near-miss bank),
    and the reason it was added in the first place -- surfacing an invented pragma name --
    is not something it actually does; see the comment on `DEFAULT_FLAGS`. Keeping it
    would buy no diagnostic and cost the exact divergence-from-the-build this function
    exists to close. It stays one `--flags` away for anyone who wants it.

    `DEFAULT_FLAGS` / `swarm.CPP_FLAGS` stay as they are, still answering a DIFFERENT
    question: `bytegate.builds_anywhere` asks "does any of 25 compilers accept this
    source at all?" (the denominator question), and #2659 kept it on the sweep's flags on
    purpose. The near-miss evaluator (`swarm.oracle_check`, and `nearmiss_db` through it)
    is still on them too; re-scored under this function's flags on 2026-09-13, all 17
    C++ rows in `nearmiss/db.jsonl` produce a byte-identical function `.text` except the
    one above, so moving it is a correctness cleanup and not a live defect.
    """
    # Lazy: build_pin imports this module, so importing it at module scope would be a
    # cycle. Unguarded on purpose -- a match verdict compiled with fallback flags is the
    # defect this closes, so it fails loudly rather than guessing a string.
    import build_pin as BP
    return BP.flags_for(path, text)


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
        # A SUCCESSFUL compile can still be telling you the lever you are measuring is not
        # being applied, so surface mwccarm's "illegal #pragma" line even on exit 0.
        #
        # Read it correctly, though. It means the pragma NAME IS KNOWN and its ARGUMENT is
        # not. It never fires on an unknown pragma name -- mwccarm accepts one of those in
        # total silence under every -w setting, `-w all` included. The line printed here
        # used to claim the opposite ("mwccarm does NOT know this pragma"), which is
        # backwards, and that reading is how sweeps came to bank levers the compiler never
        # applied. Absence of this warning proves nothing; hash-screen the object. See
        # DEFAULT_FLAGS above for the measurements and for the probe that does answer
        # "does this pragma exist?".
        for line in (r.stdout + r.stderr).splitlines():
            if "illegal #pragma" in line:
                print(f"  ! {line.strip()}  <- mwccarm knows this pragma but REJECTED the "
                      f"argument, so it is not being applied. (An unknown pragma NAME "
                      f"warns about nothing at all; silence is not evidence.)")
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
    ap.add_argument("--flags", default=None,
                    help="override the compiler flags; default is the ROM build's own "
                         "string for this file (build_flags/build_pin.flags_for), NOT "
                         "match.DEFAULT_FLAGS -- see build_flags()'s docstring")
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
    # Default to the flags the ROM BUILD will hand this file, resolved through the one
    # helper that defines them (build_flags -> build_pin.flags_for). It already applies
    # the `//cpp` first-line rule, which is what decides C vs C++ -- never the extension.
    #
    # Compiling this gate with anything else measures a build nobody ships: the same
    # one-flag gap banked func_ov006_020ea914 as a divergence-7 near-miss when its
    # already-written draft was byte-exact. See build_flags().
    #
    # An explicit --flags is taken verbatim, minus the same //cpp language swap the
    # default gets, so a deliberate lever sweep can still pin any string it likes.
    if args.flags is None:
        flags = build_flags(cfile)
    else:
        flags = args.flags
        try:
            if cfile.read_text(encoding="utf-8").startswith("//cpp") and "-lang c99" in flags:
                flags = flags.replace("-lang c99", "-lang c++")
        except OSError:
            pass  # a missing/unreadable candidate surfaces later at compile_c with a clearer error
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
