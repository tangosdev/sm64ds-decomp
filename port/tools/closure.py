#!/usr/bin/env python3
"""Measure a slice's link closure without paying for a link.

WHY THIS EXISTS. Every seat lane answers the same question over and over:
"if I add these TUs, what will the linker still want, and what is already
defined?" The lanes have been paying for that answer with REAL LINK WAVES --
the MG1 pathfinder ran FOUR of them to close a 276-TU slice and a fifth to
enumerate its wall, the S1 spine ran link rounds 4 through 8 (reported by that lane, not independently measured), and SL0
finally wrote the cheap version as a worktree-local script
(C:/tmp/g2/build/closure.py). This is that script promoted: compile the
candidate TUs with walk_window's own flags, read each object's symbol
table, and diff the union against the current walk_window.map.

WHAT IT REPORTS, in three sections:

  COMPILE FAILURES   TUs cl.exe refused, with the tail of each log. These
                     are data, not noise -- the gate-16 PMF wall and the
                     C2733 declaration conflicts announce themselves here
                     before any link would have found them. A .c whose
                     first line is the //cpp marker is compiled /TP, the
                     same reading as CMakeLists' LANGUAGE CXX block, so a
                     C2059 from probing C++ as C is a tool artifact this
                     probe no longer manufactures.
  UNRESOLVED         externals no object in the set defines and the image
                     does not carry. AN ESTIMATE OF THE FRONTIER, NOT A
                     COUNT: measured against real links it has erred in
                     BOTH directions. On the gate-36 slice it was exact;
                     on slice_ov007 it read one HIGH, because an
                     /alternatename in some other object resolves a name
                     this probe can only see as missing -- aliases live in
                     pragmas, not in symbol tables. Confirm with a link
                     before quoting.
  DUP-DEF CANDIDATES symbols the set defines that the image also defines,
  NEED A REAL LINK   and the name says the whole discipline: a symbol-table
                     probe CANNOT tell a real collision from a TU that is
                     already compiled into the image, because re-listing
                     such a TU on another slice is deduplicated by CMake,
                     not collided. Each row is therefore classified by the
                     map's own object column: a row whose defining object
                     IS the probed TU is already-in-the-image; a row
                     defined by a FOREIGN object is an LNK2005 candidate.
                     Spellings that link any-COMDAT (__xmm@, __real@,
                     ??_C@ constants) are skipped outright: identical
                     contributions merge and can never be LNK2005. ONLY A
                     REAL LINK CONVICTS. The gate-36 probe surfaced
                     fifteen candidates; fourteen were already-in-the-
                     image rows, and the planning map's ONE collision
                     (LoadArchive, whose C name hal/scene_boot.cpp
                     defines) was the correct count all along. The
                     fifteen, misread as a count, mis-scoped a successor
                     lane; this section is named what it is so that
                     cannot happen twice.

TWO MEASUREMENTS THAT READ ALIKE AND ARE NOT THE SAME. On the SL0 tree
both of these print 28 UNRESOLVED, with only 14 symbols in common:

    single TU   src/_ZN5Stage13InitResourcesEv.cpp     that TU's OWN wants
    slice       port/slice_gate36.txt (33 lines)       the UNION's wants

Label which probe set a report quotes; "the closure is 28" with the set
left unsaid is how two documents end up arguing over different numbers.

Names are compared RAW, exactly as dumpbin and the map spell them.
linkage.py's old map_symbols() lstrip("_") bug made _ZN* host definitions
invisible by over-stripping; nothing here strips anything.

    python port/tools/closure.py --root C:/tmp/g2 src/_ZN5Stage13InitResourcesEv.cpp
    python port/tools/closure.py --root C:/tmp/g2 --slice port/slice_gate36.txt
    python port/tools/closure.py --selftest
"""

import argparse
import os
import pathlib
import re
import subprocess
import sys
import msvc_env
import tempfile

# The compile line is walk_window's own, transcribed from the lane probe
# (C:/tmp/g2/build/probe.cmd) which transcribed it from build.ninja. If the
# port's flags change in CMakeLists.txt this line follows them or the
# measurement is of some other build.
CL_FLAGS = ("/nologo /c /DWIN32 /D_WINDOWS /GR /EHsc /O2 /Ob2 /DNDEBUG "
            "-std:c++17 -MT /Oy- /wd4311 /wd4312 /wd4068 /wd4576 "
            "-DPORT_CXX_ALIASES_LINKED -DSM64DS_PLATFORM_PC")



# One map row: "  0001:00012345  _name  10012345  obj". Same expression the
# SL0 probe used; it is deliberately anchored on the section:offset pair so
# header lines and the static-symbol tail never match.
MAP_ROW = re.compile(
    r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+(\S+)\s+[0-9a-fA-F]{8}")


def find_vcvars():
    try:
        return str(msvc_env.discover_vcvars())
    except msvc_env.ToolchainError:
        return None



# Spellings the linker folds under any-COMDAT selection: every TU that uses
# the constant emits an identical copy and the linker keeps one. They can
# never be LNK2005, so they are noise in a duplicate-definition list.
ANY_COMDAT = ("__xmm@", "__ymm@", "__real@", "??_C@")


def is_any_comdat(name):
    return name.startswith(ANY_COMDAT)


def stem(path):
    """scene_boot.cpp.obj and hal/scene_boot.cpp both -> scene_boot."""
    base = str(path).replace("\\", "/").rsplit("/", 1)[-1]
    for suffix in (".obj", ".cpp", ".cc", ".c"):
        if base.endswith(suffix):
            base = base[:-len(suffix)]
    return base


def map_defined(mappath):
    """{symbol: defining object basename} for every image definition.

    The object column is the LAST token of the row; it is what separates an
    already-in-the-image row from a genuine LNK2005 candidate, so it rides
    along with the name everywhere the set used to travel alone.

    REFUSES a zero-byte or truncated map LOUDLY instead of returning an
    empty universe. A FAILED LINK TRUNCATES walk_window.map -- the exact
    file this tool and facegen measure against -- and the first version
    checked only existence, so against a zeroed map it reported every
    symbol unresolved under a quiet "map defines 0 symbols" line
    (mg_fanout_costs.txt s10 finding 3). The truncation test is the map's
    own trailer: an MSVC map that finished writing carries an
    "entry point at" line AFTER its publics (walk_window.map: line 40508
    of 40510+), so a cut anywhere in the symbol bulk loses it. A cut
    inside the short static-symbol tail after the trailer is invisible to
    this test; the observed failure mode (zeroing) and any mid-publics cut
    are what it catches. No minimum-size rule here: linkage.map_symbols
    keeps its 4096-byte floor for the walk_window path, but this function
    also reads fabricated selftest maps, and the trailer is the honest
    shape test at any size. This guard sits in the PARSER, not in main(),
    so facegen's --map path inherits it through the same call it already
    makes.
    """
    p = pathlib.Path(mappath)
    if not p.is_file():
        sys.exit("closure: no map at %s -- the link did not run" % mappath)
    size = p.stat().st_size
    if size == 0:
        sys.exit("closure: %s is ZERO BYTES -- a failed link truncates the "
                 "map, and measuring against it calls every symbol "
                 "unresolved. Relink, or point --map at a copy saved before "
                 "the probe link." % mappath)
    defined, saw_trailer = {}, False
    with open(mappath, encoding="utf-8", errors="replace") as f:
        for line in f:
            m = MAP_ROW.match(line)
            if m and m.group(1) not in defined:
                defined[m.group(1)] = line.split()[-1]
            elif "entry point at" in line:
                saw_trailer = True
    if not defined:
        sys.exit("closure: %s is %d bytes with NO symbol rows -- a stub "
                 "from a failed link, not a map to measure" % (mappath, size))
    if not saw_trailer:
        sys.exit("closure: %s has symbol rows but no 'entry point at' "
                 "trailer, so it stopped mid-write -- a TRUNCATED map, not "
                 "a linked build. Relink, or use a saved copy." % mappath)
    return defined


def read_syms(symspath):
    """dumpbin /SYMBOLS -> (undefined externals, defined externals)."""
    und, dfn = set(), set()
    with open(symspath, encoding="utf-8", errors="replace") as f:
        for line in f:
            if "External" not in line:
                continue
            m = re.search(r"\|\s+(\S+)", line)
            if not m:
                continue
            if "UNDEF" in line:
                und.add(m.group(1))
            else:
                dfn.add(m.group(1))
    return und, dfn


def slice_lines(path):
    out = []
    with open(path, encoding="utf-8-sig", errors="replace") as f:
        for line in f:
            line = line.split("#", 1)[0].strip()
            if line:
                out.append(line)
    return out


def compile_batch(root, sources, outdir, extra_flags=""):
    """Compile every source with the port's flags; one vcvars call total.

    Returns {source: syms-file-or-None}; the .log beside each object holds
    cl's own words for the failures.
    """
    vcvars = find_vcvars()
    if vcvars is None:
        sys.exit("no installed x86 MSVC environment found; "
                 "closure needs cl.exe and dumpbin")
    outdir = pathlib.Path(outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    root = pathlib.Path(root)
    inc = "-I%s -I%s -I%s" % (root / "port", root / "include",
                              root / "port" / "ntr" / "include")
    rootdef = '-DPORT_REPO_ROOT="%s"' % str(root).replace("\\", "/")

    script = [u"@echo off", u'call "%s" >nul' % vcvars]
    plan = []
    for i, src in enumerate(sources):
        srcp = pathlib.Path(src)
        if not srcp.is_absolute():
            srcp = root / srcp
        name = "%03d_%s" % (i, srcp.name)
        obj = outdir / (name + ".obj")
        syms = outdir / (name + ".syms")
        log = outdir / (name + ".log")
        plan.append((src, srcp, obj, syms))
        # A .c whose first line is the //cpp marker is C++ in the port's
        # build (CMakeLists' LANGUAGE CXX block); probe it the same way or
        # the C2059 this probe reports is its own artifact and the TU's
        # symbols poison both output sections.
        lang = ""
        try:
            with open(srcp, encoding="utf-8-sig", errors="replace") as f:
                if f.readline().strip().startswith("//cpp"):
                    lang = "/TP "
        except OSError:
            pass
        script.append('cl %s %s%s %s %s /Fo"%s" "%s" > "%s" 2>&1'
                      % (CL_FLAGS, lang, extra_flags, rootdef, inc, obj,
                         srcp, log))
        script.append('if exist "%s" dumpbin /nologo /SYMBOLS "%s" > "%s"'
                      % (obj, obj, syms))
    runner = outdir / "closure_run.cmd"
    runner.write_text("\r\n".join(script) + "\r\n", encoding="ascii",
                      errors="replace")
    subprocess.run(["cmd", "/c", str(runner)], capture_output=True)

    results = {}
    for src, srcp, obj, syms in plan:
        results[src] = syms if syms.exists() and obj.exists() else None
    return results


def measure(root, mappath, sources, outdir, extra_flags=""):
    have = map_defined(mappath)
    results = compile_batch(root, sources, outdir, extra_flags)
    allund, alldef, failed = set(), set(), []
    def_by = {}
    for src, syms in results.items():
        if syms is None:
            log = pathlib.Path(str(pathlib.Path(outdir)))
            tail = []
            for cand in log.glob("*_%s.log" % pathlib.Path(src).name):
                tail = cand.read_text(errors="replace").splitlines()[-3:]
            failed.append((src, tail))
            continue
        u, d = read_syms(syms)
        allund |= u
        alldef |= d
        for sym in d:
            def_by.setdefault(sym, set()).add(stem(src))
    unresolved = sorted(x for x in allund if x not in have
                        and x not in alldef)
    dup = {"own": [], "foreign": [], "comdat": []}
    for sym in sorted(x for x in alldef if x in have):
        if is_any_comdat(sym):
            dup["comdat"].append(sym)
        elif stem(have[sym]) in def_by.get(sym, set()):
            dup["own"].append((sym, have[sym]))
        else:
            dup["foreign"].append((sym, have[sym]))
    return have, failed, unresolved, dup


def report(have, failed, unresolved, dup):
    print("map defines %d symbols" % len(have))
    if failed:
        print("\n=== COMPILE FAILURES (%d) ===" % len(failed))
        for src, tail in failed:
            print("  ", src)
            for line in tail:
                print("       ", line)
    print("\n=== UNRESOLVED after this slice: %d "
          "(an estimate; confirm with a link) ===" % len(unresolved))
    for x in unresolved:
        print("   ", x)
    print("\n=== DUP-DEF CANDIDATES, NEED A REAL LINK ===")
    print("  already in the image from its own TU: %d" % len(dup["own"]))
    for sym, obj in dup["own"]:
        print("    %s  --  %s compiles it already" % (sym, obj))
    print("  FOREIGN-object rows, the LNK2005 candidates: %d"
          % len(dup["foreign"]))
    for sym, obj in dup["foreign"]:
        print("    %s  --  image copy is from %s" % (sym, obj))
    if dup["comdat"]:
        print("  skipped any-COMDAT spellings (never LNK2005): %d"
              % len(dup["comdat"]))


def selftest():
    """Four tiny TUs against a fabricated map; every section exercised.

    a.cpp defines a_def (which the fabricated map ALSO defines, from
    a.cpp.obj: the already-in-the-image case) and wants missing_sym,
    present_sym and dup_sym. b.cpp defines dup_sym, which the map carries
    from a FOREIGN object: the real LNK2005 candidate. c.cpp does not
    compile. d.c is C++ behind the //cpp marker and compiles only because
    the probe honours it with /TP. Expected: failures == {c.cpp},
    unresolved == {_missing_sym} (satisfied-in-set d_def proves the
    set-union rule, satisfied-in-map present_sym proves the map rule),
    own == {_a_def}, foreign == {_dup_sym}. The any-COMDAT skip is
    asserted on the predicate directly: fabricating an __xmm@ emission
    from a toy TU is not worth the flakiness.
    """
    with tempfile.TemporaryDirectory() as td:
        tdp = pathlib.Path(td)
        (tdp / "port").mkdir()
        (tdp / "include").mkdir()
        (tdp / "port" / "ntr" / "include").mkdir(parents=True)
        (tdp / "a.cpp").write_text(
            'extern "C" void missing_sym(); extern "C" void present_sym();\n'
            'extern "C" void dup_sym(); extern "C" void d_def();\n'
            'extern "C" void a_def() { missing_sym(); present_sym(); '
            'dup_sym(); d_def(); }\n')
        (tdp / "b.cpp").write_text('extern "C" void dup_sym() {}\n')
        (tdp / "c.cpp").write_text("this does not compile\n")
        (tdp / "d.c").write_text(
            '//cpp\n'
            'extern "C" { struct D { void m() {} };\n'
            'void d_def() { D d; d.m(); } }\n')
        fakemap = tdp / "fake.map"
        # The trailer line is part of the fabrication ON PURPOSE: map_defined
        # refuses a map without it (finding 3), so a fake map that omitted it
        # would fail the guard the way a truncated real one does.
        fakemap.write_text(
            " 0001:00000000       _present_sym               10000000 f"
            "   x.obj\n"
            " 0001:00000010       _dup_sym                   10000010 f"
            "   x.obj\n"
            " 0001:00000020       _a_def                     10000020 f"
            "   a.cpp.obj\n"
            "\n"
            " entry point at        0001:00000000\n")
        have, failed, unresolved, dup = measure(
            tdp, fakemap,
            [str(tdp / "a.cpp"), str(tdp / "b.cpp"), str(tdp / "c.cpp"),
             str(tdp / "d.c")],
            tdp / "probe")
        ok = True

        def expect(cond, what, got):
            nonlocal ok
            if not cond:
                print("FAIL: %s: %s" % (what, got))
                ok = False

        expect([s for s, _ in failed] == [str(tdp / "c.cpp")],
               "compile-failure section", failed)
        expect(unresolved == ["_missing_sym"], "unresolved", unresolved)
        expect([s for s, _ in dup["own"]] == ["_a_def"],
               "already-in-the-image rows", dup["own"])
        expect([s for s, _ in dup["foreign"]] == ["_dup_sym"],
               "foreign LNK2005 candidates", dup["foreign"])
        expect(dup["comdat"] == [], "comdat rows", dup["comdat"])
        expect(is_any_comdat("__xmm@0000ffff") and
               is_any_comdat("__real@3f800000") and
               is_any_comdat("??_C@_0BA@abcd@") and
               not is_any_comdat("_LoadArchive"),
               "any-COMDAT predicate", "pattern set")

        # Finding 3: a failed link zeroes walk_window.map, both tools'
        # universe, and this tool used to answer with "map defines 0
        # symbols" and every symbol unresolved. Three refusal shapes, each
        # asserted on its own message so a regression cannot hide behind a
        # neighbouring one.
        def expect_refusal(path, needle, what):
            try:
                map_defined(path)
            except SystemExit as e:
                expect(needle in str(e),
                       what + " refusal message", str(e))
            else:
                expect(False, what + " was ACCEPTED", "no SystemExit")

        zmap = tdp / "zero.map"
        zmap.write_text("")
        expect_refusal(zmap, "ZERO BYTES", "zero-byte map")

        stub = tdp / "stub.map"
        stub.write_text(" walk_window\n\n Timestamp is 68a01c2e\n"
                        " Preferred load address is 00400000\n")
        expect_refusal(stub, "NO symbol rows", "row-less stub map")

        cut = tdp / "cut.map"
        cut.write_text(
            " 0001:00000000       _present_sym               10000000 f"
            "   x.obj\n"
            " 0001:00000010       _dup_sym                   10000010 f"
            "   x.obj\n")
        expect_refusal(cut, "TRUNCATED", "trailer-less (truncated) map")

        print("selftest %s" % ("PASS" if ok else "FAIL"))
        return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(
        description="slice closure without a link; see the docstring")
    ap.add_argument("sources", nargs="*",
                    help="TU paths, absolute or root-relative")
    ap.add_argument("--root", default=None,
                    help="repo root (default: two dirs above this file)")
    ap.add_argument("--map", dest="mapfile", default=None,
                    help="image map (default: <root>/build/port/"
                         "walk_window.map)")
    ap.add_argument("--slice", action="append", default=[],
                    help="read TU paths from a slice_*.txt (repeatable)")
    ap.add_argument("--out", default=None,
                    help="object/scratch dir (default: a temp dir)")
    ap.add_argument("--extra-flags", default="",
                    help="appended verbatim to the cl line")
    ap.add_argument("--selftest", action="store_true")
    args = ap.parse_args()

    if args.selftest:
        sys.exit(selftest())

    root = pathlib.Path(args.root) if args.root else \
        pathlib.Path(__file__).resolve().parents[2]
    mapfile = args.mapfile or (root / "build" / "port" / "walk_window.map")
    if not pathlib.Path(mapfile).exists():
        sys.exit("no map at %s; build walk_window first or pass --map"
                 % mapfile)
    sources = list(args.sources)
    for s in args.slice:
        sources += slice_lines(s)
    if not sources:
        sys.exit("nothing to measure: pass TU paths or --slice")

    if args.out:
        report(*measure(root, mapfile, sources, args.out,
                        args.extra_flags))
    else:
        with tempfile.TemporaryDirectory() as td:
            report(*measure(root, mapfile, sources, td, args.extra_flags))


if __name__ == "__main__":
    main()
