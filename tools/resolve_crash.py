#!/usr/bin/env python3
"""Resolve a port crash dump's module offsets to symbol+offset.

The port's crash path (port/tests/fault_probe.h) writes module-RELATIVE
addresses -- "+0x0002xxxx" tokens -- because the in-crash writer has no symbol
table and cannot afford one. This tool does the symbol resolution OFFLINE, so
the in-crash path stays a raw-Win32 buffer write with no dependencies.

Feed it a dump file (crash.txt, exit.txt, or one of the rolling
%TEMP%\\sm64ds-crashes\\crash-*.txt files) and it rewrites every "+0xNNNNNNNN"
offset in place with the nearest preceding symbol from the linker /MAP file,
e.g.

    offset    +0x00024f10   ->   func_02043fdc +0x90   (func_02043fdc_hostcopy.cpp.obj)

The map is build/port/walk_window.map by default (the build writes it via /MAP).

Usage:
    python tools/resolve_crash.py <dumpfile> [--map build/port/walk_window.map]
    python tools/resolve_crash.py --latest        # newest dump in %TEMP%
    python tools/resolve_crash.py <dumpfile> --quarantine   # also a quarantine.log
    python tools/resolve_crash.py --selftest

Resolution model: the MAP header names "Preferred load address is 00400000".
Each symbol row carries a preferred VA in its 4th column; the module-relative
RVA is (preferred_VA - preferred_load_address), which is exactly the "+offset"
the dump prints (ExceptionAddress - GetModuleHandle(0)). So a dump offset maps
to the symbol with the greatest RVA <= offset.

NOT EVERY DUMP OFFSET IS IN THE MODULE. The dump subtracts the module base
from EVERY address it prints -- return addresses inside system DLLs included,
and on an execute-at-stack fault the faulting eip itself. Report 7447e46c
(level 46) faulted at eip 00e7f1bc, a stack address, and the "+00a7f1bc" it
printed is not an image offset at all; the nearest-preceding rule still
answered it, with the last code symbol in the map and a delta of 8.6 MB, in
the same column and format as the genuine stack frames below it. So two
refusals guard every resolution now: an offset past the image's end
(SizeOfImage, read from the exe next to the map, same as
port/tools/faultmap.py) is refused outright, and a resolution implausibly far
past the symbol it matched is refused even inside the image, where the data
sections sit past the last code symbol.
"""
import argparse
import glob
import os
import re
import sys

# the dump writes "+NNNNNNNN" (8 hex, no 0x); crash.txt/exit.txt also print a
# "+0xNNNNNNNN" style in a couple of spots, so accept both.
OFF_RE = re.compile(r"\+(?:0x)?([0-9a-fA-F]{8})\b")
# a MAP symbol row:  0001:00003c20       _main            00404c20 f   walk_window.cpp.obj
SYM_RE = re.compile(
    r"^\s*[0-9a-fA-F]{4}:[0-9a-fA-F]{8}\s+"
    r"(?P<name>\S+)\s+"
    r"(?P<va>[0-9a-fA-F]{8})\s+"
    r"(?P<kind>[fi])\s+"
    r"(?P<obj>\S+)"
)
LOAD_RE = re.compile(r"Preferred load address is\s+([0-9a-fA-F]+)")

# A resolution more than this past its symbol is refused. Between two code
# symbols the bisect bounds the distance by the real gap, and the widest gap
# measured on the linked map is 0x1dff0 (the tail of _port_ov006_pack_check);
# at double that, this can only fire past the LAST code symbol, where nothing
# else bounds the distance and report 7447e46c's stack eip resolved +0x83657c
# past bind_from_vram's ??__Fseen. Mirrored in port/tools/faultmap.py
# MAX_SYM_DELTA -- change both together.
MAX_CODE_DELTA = 0x40000


def parse_map(text):
    """Return (sorted [(rva, name, obj)], load_base). Only code symbols (f)."""
    m = LOAD_RE.search(text)
    load_base = int(m.group(1), 16) if m else 0x400000
    syms = []
    for line in text.splitlines():
        mm = SYM_RE.match(line)
        if not mm:
            continue
        if mm.group("kind") != "f":
            continue  # code symbols only; data offsets are not return addresses
        va = int(mm.group("va"), 16)
        rva = va - load_base
        if rva < 0:
            continue
        name = mm.group("name")
        # strip the leading underscore MSVC decoration on cdecl C names
        if name.startswith("_") and not name.startswith("__"):
            name = name[1:]
        syms.append((rva, name, mm.group("obj")))
    syms.sort()
    return syms, load_base


def load_map(map_path):
    with open(map_path, "r", errors="replace") as f:
        return parse_map(f.read())


def image_size(map_path):
    """SizeOfImage from the PE next to the map, or None.

    The bound that lets an out-of-module offset be REFUSED instead of
    resolved. Same reader as port/tools/faultmap.py image_size -- change both
    together.
    """
    exe = os.path.splitext(map_path)[0] + ".exe"
    try:
        with open(exe, "rb") as f:
            head = f.read(0x400)
        if head[:2] != b"MZ":
            return None
        pe = int.from_bytes(head[0x3C:0x40], "little")
        if head[pe:pe + 4] != b"PE\0\0":
            return None
        opt = pe + 24
        return int.from_bytes(head[opt + 56:opt + 60], "little")
    except (OSError, ValueError, IndexError):
        return None


def resolve_one(offset, syms):
    """Nearest preceding code symbol for a module RVA. Binary search.

    Identical-code folding gives several symbols one address; this returns
    whichever folded sibling sorts LAST and says nothing about the others.
    Fine for a quick inline annotation, wrong to lean on for blame --
    port/tools/faultmap.py prints every folded name and says FOLDED, so use
    that when the attribution matters.
    """
    lo, hi, best = 0, len(syms) - 1, None
    while lo <= hi:
        mid = (lo + hi) // 2
        if syms[mid][0] <= offset:
            best = mid
            lo = mid + 1
        else:
            hi = mid - 1
    if best is None:
        return None
    rva, name, obj = syms[best]
    return name, offset - rva, obj


def annotate(line, syms, size=None):
    """Append resolved symbol names to every +offset token on a line.

    An offset the map cannot honestly answer gets a refusal in the slot a
    resolution would occupy, never a confident wrong name.
    """
    hits = []
    for m in OFF_RE.finditer(line):
        off = int(m.group(1), 16)
        if size is not None and off >= size:
            hits.append("<outside the module: image ends at +0x%x, "
                        "not resolvable>" % size)
            continue
        r = resolve_one(off, syms)
        if not r:
            continue
        name, delta, obj = r
        if delta > MAX_CODE_DELTA:
            hits.append("<+0x%x past the nearest code symbol (%s): "
                        "implausibly far, not resolvable>" % (delta, name))
            continue
        hits.append("%s +0x%x (%s)" % (name, delta, obj))
    if not hits:
        return line.rstrip("\n")
    return "%s   ->   %s" % (line.rstrip("\n"), " | ".join(hits))


def newest_dump():
    tmp = os.environ.get("TEMP") or os.environ.get("TMP") or "."
    d = os.path.join(tmp, "sm64ds-crashes")
    cands = glob.glob(os.path.join(d, "crash-*.txt"))
    if not cands:
        return None
    return max(cands, key=os.path.getmtime)


# ---------------------------------------------------------------------------
# selftest: fixture map, so the refusal arms are pinned (the style of
# port/tools/abicheck.py --selftest)
# ---------------------------------------------------------------------------
FIX_MAP = """\
 Preferred load address is 00400000

  0001:00001000       _bootstrap                 00401000 f   boot.cpp.obj
  0001:00001100       ?Render@Trap@@QAEHXZ       00401100 f   trap.cpp.obj
  0001:00002000       __last_code                00402000 f   tail.cpp.obj
  0002:00000000       _table_data                00500000     data.cpp.obj
"""
# fixture image: code ends at +0x2000, the data tail runs out to here
FIX_SIZE = 0x100000


def selftest():
    bad = 0
    print("resolve_crash --selftest")
    syms, base = parse_map(FIX_MAP)

    def want(note, got, needle, absent=None):
        nonlocal bad
        ok = needle in got and (absent is None or absent not in got)
        bad += 0 if ok else 1
        print("  %-4s %s" % ("ok" if ok else "FAIL", note))
        if not ok:
            print("       got: %s" % got)

    ok = base == 0x400000 and len(syms) == 3
    bad += 0 if ok else 1
    print("  %-4s fixture parses to 3 code symbols at base 0x400000, the "
          "data row does not join" % ("ok" if ok else "FAIL"))

    # a real in-code offset resolves exactly as before
    want("an in-code offset resolves to symbol+delta",
         annotate("frame 0: +0x00001104", syms, FIX_SIZE),
         "?Render@Trap@@QAEHXZ +0x4 (trap.cpp.obj)")

    # THE FLAGGED SHAPE, report 7447e46c: an execute-at-stack eip whose
    # "offset" is past the image end. Refused, and no symbol is named.
    want("an offset past the image end is refused, no symbol printed",
         annotate("eip: +0x00200000", syms, FIX_SIZE),
         "<outside the module: image ends at +0x100000, not resolvable>",
         absent="__last_code")

    # inside the image but far past the last CODE symbol -- the data tail.
    # A code attribution there is exactly as wrong as one past the end.
    want("an in-image offset far past the last code symbol is refused",
         annotate("stack[03] +0x00050000", syms, FIX_SIZE),
         "implausibly far, not resolvable", absent="(tail.cpp.obj)")

    # no exe next to the map: size is None and the distance bound is the only
    # guard. It must still catch the same offset.
    want("size unknown: the distance bound alone still refuses",
         annotate("stack[04] +0x00200000", syms, None),
         "implausibly far, not resolvable", absent="(tail.cpp.obj)")

    # and a sane delta stays resolvable with no size, so the guard cannot
    # eat real resolutions when the exe is missing
    want("size unknown: a sane delta still resolves",
         annotate("frame 1: +0x00002010", syms, None),
         "__last_code +0x10 (tail.cpp.obj)")

    print("SELFTEST %s" % ("PASSED" if not bad else "FAILED (%d)" % bad))
    return 1 if bad else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("dump", nargs="?", help="dump file to resolve")
    ap.add_argument("--map", default=None,
                    help="linker /MAP (default build/port/walk_window.map)")
    ap.add_argument("--latest", action="store_true",
                    help="resolve the newest dump under %%TEMP%%\\sm64ds-crashes")
    ap.add_argument("--quarantine", action="store_true",
                    help="also annotate a quarantine.log (same +offset tokens)")
    ap.add_argument("--selftest", action="store_true",
                    help="run the fixture selftest and exit")
    args = ap.parse_args()

    if args.selftest:
        return selftest()

    dump = args.dump
    if args.latest:
        dump = newest_dump()
        if not dump:
            print("no dumps found under %TEMP%\\sm64ds-crashes", file=sys.stderr)
            return 2
        print("# latest dump: %s" % dump, file=sys.stderr)
    if not dump:
        ap.print_help()
        return 2

    map_path = args.map
    if not map_path:
        here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        map_path = os.path.join(here, "build", "port", "walk_window.map")
    if not os.path.exists(map_path):
        print("map not found: %s (build the port first)" % map_path,
              file=sys.stderr)
        return 2

    syms, base = load_map(map_path)
    size = image_size(map_path)
    print("# resolved against %s (%d code symbols, base 0x%x)"
          % (os.path.basename(map_path), len(syms), base), file=sys.stderr)
    if size is not None:
        print("# image size 0x%x from the exe next to the map; offsets past "
              "it are refused" % size, file=sys.stderr)
    else:
        print("# no readable exe next to the map: image size unknown, "
              "out-of-module offsets are caught only by the +0x%x distance "
              "bound" % MAX_CODE_DELTA, file=sys.stderr)

    with open(dump, "r", errors="replace") as f:
        for line in f:
            print(annotate(line, syms, size))
    return 0


if __name__ == "__main__":
    sys.exit(main())
