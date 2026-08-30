"""Compile every include/*.h standalone, as C and as C++, and report broken blocks.

## The defect this exists to catch

A generated header's `#ifdef __cplusplus` block is only compiled when some `.cpp` file
includes it. Most headers are included exclusively by `.c` files, so for those the block
is dead text that nothing has ever parsed -- and the generator has put things in it that
are not C++ at all. Three found so far, all from `tools/gen_header.py`, all the same root
cause (a token copied out of a source file without checking it names anything visible
from the header) wearing three different disguises:

    ExpandingHeapAllocator.h   forwards void* Allocate(unsigned int, int);
                               ^ a fragment of the parameter name `forwards` spilled
                                 into the return type                        (#1211)

    SolidHeapAllocator.h       call ResetEnd void Reset(unsigned int);
                               ^ scraped out of the comment on Reset's body,
                                 "bit 0: call ResetStart; bit 1: call ResetEnd"

    dScene_c.h                 Bool BeforeInitResources();
                               ^ lifted from a `typedef int Bool;` that is file-local
                                 to the BeforeInitResources body, now in
                                 src/actors/Scene.cpp

None was found by a gate. Each was found by migrating a method, which compiles the block
for the first time -- so the cost of finding them was one slice each. This tool finds
them all at once, in about a minute, without touching a single source file.

## Why it compiles BOTH ways

Compiling as C++ alone cannot tell a broken block from a header that simply needs other
headers included first. The pair does:

    fails C++, passes C   -> the __cplusplus block is broken        <- the defect
    fails both            -> needs other headers first; not this
    passes both           -> clean

Flags mirror `rombuild.compile_one` -- same CFLAGS, same `-i include`, same
LM_LICENSE_FILE. A sweep compiled differently from the build would prove nothing about
the build.

One deliberate difference, and an earlier version of this docstring described it wrongly.
The BUILD does not pick the language from the file extension: `rombuild.py:275` and
`build_pin.py:127-138` both key on a leading `//cpp` marker, and build_pin says outright
that suffix-keying is the historical mistake ("53 committed .c files carry the marker
too"). This sweep authors its own one-line TUs, so it selects the language directly
rather than by either rule. That is correct for the question being asked -- "does this
header parse as C++?" -- but it is not the build's rule, and nothing here should be cited
as evidence about how the build chooses.

## What this does NOT catch

A declaration that compiles both ways but MEANS different things. `types.h` has
`typedef int bool;` under `#ifndef __cplusplus`, so a header declaring `bool Foo();` is
4 bytes wide in C and the 1-byte keyword in C++. Four headers do that today
(Animation.h, ExpandingHeap.h, Heap.h, SolidHeap.h) and all four are already included by
.cpp files without incident, so it is latent rather than live -- and `build_pin.verify`
would reject the byte mismatch if a migration ever leaned on it. Recorded here because
this is the natural place to look for it, not because it is currently wrong.

Usage:
    python tools/header_cpp_sweep.py            # report
    python tools/header_cpp_sweep.py --check    # exit 1 if any header is defective
    python tools/header_cpp_sweep.py --json out.json
"""
import argparse
import concurrent.futures
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

from rombuild import CFLAGS, INCLUDE, LICENSE, MW, VERSION, launcher  # noqa: E402

EXE = MW / VERSION / "mwccarm.exe"

BANNERS = [
    ("gen_header.py",   re.compile(r"AUTO-GENERATED.*gen_header\.py", re.S)),
    ("deepen_rtti.py",  re.compile(r"AUTO-GENERATED.*deepen_rtti\.py", re.S)),
    ("decl_headers.py", re.compile(r"AUTO-GENERATED.*decl_headers\.py", re.S)),
    ("hand-edited",     re.compile(r"Hand-edited, against evidence")),
]


def banner_of(text):
    head = text[:1200]
    for name, pat in BANNERS:
        if pat.search(head):
            return name
    return "(none)"


def _compile(hdr, cpp):
    """Compile a one-line TU that includes `hdr`. Returns (ok, first error line)."""
    flags = CFLAGS.replace("-lang c99", "-lang c++") if cpp else CFLAGS
    with tempfile.TemporaryDirectory() as td:
        src = pathlib.Path(td) / ("probe.cpp" if cpp else "probe.c")
        src.write_text('#include "%s"\n' % hdr.name, encoding="utf-8")
        obj = pathlib.Path(td) / "probe.o"
        cmd = [*launcher(), str(EXE), *flags.split(),
               "-i", str(INCLUDE), "-c", str(src), "-o", str(obj)]
        r = subprocess.run(cmd, capture_output=True, text=True,
                           env=dict(os.environ, LM_LICENSE_FILE=str(LICENSE)), cwd=td)
        if r.returncode == 0 and obj.is_file():
            return True, ""
        out = [ln.strip() for ln in (r.stdout + r.stderr).splitlines() if ln.strip()]
        # mwccarm prints "<file>:<line>: <message>" then "Errors caused tool to abort."
        # The first line carrying a colon-line-number is the useful one.
        for ln in out:
            if re.search(r"\.h:\d+:", ln):
                return False, ln
        return False, (out[0] if out else "?")


def probe(hdr):
    text = hdr.read_text(encoding="utf-8", errors="ignore")
    ok_c, err_c = _compile(hdr, cpp=False)
    ok_cpp, err_cpp = _compile(hdr, cpp=True)
    return {"name": hdr.name, "banner": banner_of(text),
            "has_cpp_block": "#ifdef __cplusplus" in text,
            "ok_c": ok_c, "err_c": err_c, "ok_cpp": ok_cpp, "err_cpp": err_cpp}


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--check", action="store_true",
                    help="exit 1 if any header's __cplusplus block is broken")
    ap.add_argument("--json", metavar="PATH", help="write the full per-header table")
    args = ap.parse_args()

    if not EXE.is_file():
        # Fail closed and say so: a sweep that silently skips is worse than none.
        print("toolchain not installed (%s) -- nothing swept" % EXE, file=sys.stderr)
        return 2

    hdrs = sorted((REPO / "include").glob("*.h"))
    with concurrent.futures.ThreadPoolExecutor(max_workers=16) as ex:
        rows = list(ex.map(probe, hdrs))

    # A header that fails C++ but passes C has a broken __cplusplus block -- but only if
    # it HAS one. Without that conjunct the bucket is mislabelled: a blockless header
    # that trips some other C++-only rule would be filed under "__cplusplus block
    # defect". Currently no such header exists, which is why the omission never showed.
    defect = [r for r in rows if not r["ok_cpp"] and r["ok_c"] and r["has_cpp_block"]]
    other_cpp_only = [r for r in rows
                      if not r["ok_cpp"] and r["ok_c"] and not r["has_cpp_block"]]
    both = [r for r in rows if not r["ok_cpp"] and not r["ok_c"]]

    clean = len(rows) - len(defect) - len(other_cpp_only) - len(both)
    print("headers swept: %d  (compiler %s)" % (len(rows), VERSION))
    print("  clean as C++                 : %d" % clean)
    print("  FAIL C++ but PASS C          : %d   <-- __cplusplus block defect" % len(defect))
    print("  FAIL C++ but PASS C, no block: %d   <-- C++-only defect, not in a block"
          % len(other_cpp_only))
    print("  fail both                    : %d   <-- NOT a free pass, see below"
          % len(both))

    withblk = [r for r in rows if r["has_cpp_block"]]
    print("\nheaders carrying a __cplusplus block: %d" % len(withblk))
    pop = {}
    for r in withblk:
        pop.setdefault(r["banner"], [0, 0])
        pop[r["banner"]][0] += 1
        if not r["ok_cpp"] and r["ok_c"]:
            pop[r["banner"]][1] += 1
    for b in sorted(pop, key=lambda k: -pop[k][0]):
        tot, bad = pop[b]
        print("  %-16s %3d with block, %3d defective" % (b, tot, bad))

    for label, rowset in (("DEFECTIVE -- the __cplusplus block does not compile:", defect),
                          ("C++-ONLY FAILURE outside any __cplusplus block:", other_cpp_only),
                          ("FAILS AS C TOO -- coverage hole, see below:", both)):
        if rowset:
            print("\n" + label)
            for r in sorted(rowset, key=lambda r: r["name"]):
                print("  %-34s %-15s %s" % (r["name"], r["banner"], r["err_cpp"][:90]))

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(rows, indent=1))
        print("\nwrote %s" % args.json)

    # THE FAILS-BOTH BUCKET IS NOT A FREE PASS, and the first version of this tool
    # treated it as one. A header that needs other headers included first AND has a
    # broken __cplusplus block fails both ways, so it lands in that bucket and this
    # sweep can say nothing about its block -- forever, and silently. The count is
    # currently 0, which is what makes the "three defects, ever" claim sound; so it is
    # ratcheted at 0 rather than baselined. If one appears, either fix the header's
    # includes so it compiles standalone, or the sweep has stopped covering it.
    bad = len(defect) + len(other_cpp_only) + len(both)
    if args.check and bad:
        print("\nheader-cpp-sweep: FAIL (%d defective, %d C++-only, %d uncoverable)"
              % (len(defect), len(other_cpp_only), len(both)), file=sys.stderr)
        return 1
    print("\nheader-cpp-sweep: %s" % ("PASS" if not bad else "reported"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
