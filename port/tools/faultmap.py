#!/usr/bin/env python3
"""Turn a port fault line back into source: symbol, object file, actor class.

The port's fault probe prints module-relative offsets, because that is the only
thing it can print that survives ASLR:

    FAULT code c0000005 at +0x0008e07f accessing 00000004
      walker node 00a1b2c0 actor 0121ff40 id 0xec
      frame 0: +0x0008e07f
      ...
      stack[07] +0x000441aa

Every one of those `+0x...` numbers is `address - GetModuleHandleA(0)`, and
walk_window.map turns each back into a name. Doing that by hand means opening a
2.7 MB map, converting the Rva+Base column, and eyeballing for the last symbol
below the offset. This does it, for a single offset or for a whole crash.txt /
playlog at once.

    python port/tools/faultmap.py +0x0003c75c
    python port/tools/faultmap.py 0x3c75c 0x441aa          (several at once)
    python port/tools/faultmap.py path/to/crash.txt        (parse a dump)
    python port/tools/faultmap.py --id 0xec                (decode an actor id)
    python port/tools/faultmap.py --map OTHER.map +0x1234  (another build's map)
    python port/tools/faultmap.py --all +0x3ec80           (list every folded name)
    python port/tools/faultmap.py --selftest

With no --map this reads build/port/walk_window.map from the checkout the
SCRIPT lives in, not the current directory. Same rule as linkage.py: a tool
rooted at CWD reads right from the repo root and silently symbolizes another
tree's binary from anywhere else, which on a crash report is the worst possible
kind of wrong answer -- confident, specific, and about someone else's build.

THE OFFSET CONVERSION. An MSVC map row is

    0001:0003dc80       ?Render@Trap@@QAEHXZ       0043ec80 f   <obj>

and the first column is a SECTION-relative offset (0x3dc80), while the fault
line is IMAGE-relative. The image offset is the third column minus the
"Preferred load address" from the map header: 0x43ec80 - 0x400000 = 0x3ec80.
Bisecting on the first column instead is off by the section's own base, which
lands you a few functions away and reads perfectly plausible.

MSVC FOLDS IDENTICAL BODIES, SO ONE ADDRESS CAN CARRY SEVERAL NAMES.

Identical COMDAT folding is on in this link, and two methods that compile to
the same bytes are given the same address. On the 4888-linked baseline

    0001:0003dc80       ?Render@MontyMole@@QAEHXZ  0043ec80 f  _ZN9MontyMole6RenderEv.cpp.obj
    0001:0003dc80       ?Render@Trap@@QAEHXZ       0043ec80 f  _ZN4Trap6RenderEv.cpp.obj

are one function. A symbolizer that takes the first row and prints "MontyMole"
has told you a fact about the wrong class half the time, and there is nothing
in the crash to contradict it. So this prints EVERY symbol sharing an address
and says FOLDED when there is more than one. Which sibling was really running
is not in the map; the walker's actor id usually settles it, which is the other
half of what this tool prints.

AN OFFSET THE MAP CANNOT ANSWER IS REFUSED, NOT RESOLVED. Past the image's
end (SizeOfImage, from the exe next to the map) an offset is a system-DLL
frame or a stack/heap address the probe made module-relative anyway, and it
reads as OUTSIDE. When the exe is missing and the size unknown, the same
class of offset is caught by distance instead: the widest genuine gap between
consecutive symbols on the linked map is 0x1dff0, so a hit implausibly far
past its symbol is refused rather than attributed. tools/resolve_crash.py
applies the same two bounds; report 7447e46c's execute-at-stack eip is the
incident both guards exist for.

THE ACTOR ID. `id 0xec` in the walker line is the ROM actor id at +0xc of the
actor object. The names come from the registry's own rows -- hal/actor_classes.inc
plus the two rows spelled directly in hal/actor_registry.cpp (PLAYER 0x0bf and
CAMERA 0x14c) -- read at run time, never copied here. A table copied into a
tool is a table that goes stale the first time a class lands, and then the tool
starts naming the wrong actor with total confidence.
"""

import bisect
import os
import re
import sys

# 0001:0003dc80       ?Render@Trap@@QAEHXZ       0043ec80 f   obj.obj
# The flag column is `f`, `f i`, or absent on data rows, and the object may be
# `lib:member.obj`, so it is anchored at end of line rather than left to a
# backtracking \S*.
ROW = re.compile(
    r"^\s+([0-9a-fA-F]{4}):([0-9a-fA-F]{8})\s+(\S+)\s+([0-9a-fA-F]{8})\s+"
    r"((?:[fi]\s+)*)(\S+)\s*$")
PREFERRED = re.compile(r"Preferred load address is ([0-9a-fA-F]+)")

# Anything that looks like a module offset the fault probe printed.
OFFSET_IN_TEXT = re.compile(r"\+0x([0-9a-fA-F]{1,8})")
# `  walker node 00a1b2c0 actor 0121ff40 id 0xec`
WALKER = re.compile(r"walker node\s+(\S+)\s+actor\s+(\S+)\s+id\s+0x([0-9a-fA-F]+)")
# The registry rows, in both files that spell one.
ACTOR_ROW = re.compile(r"\{\s*(0x[0-9a-fA-F]+|\d+)\s*,\s*\"([A-Za-z0-9_]+)\"\s*,")


def default_root():
    """The checkout this script lives in: <root>/port/tools/faultmap.py."""
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


def parse_map(lines):
    """(entries, preferred_base or None) from /MAP text lines.

    The file-level refusals (missing, tiny, header-less, row-less) live in
    load_map; this is the pure row walk, which is also what --selftest runs
    its fixture through.
    """
    base = None
    at = {}
    for line in lines:
        if base is None:
            m = PREFERRED.search(line)
            if m:
                base = int(m.group(1), 16)
                continue
        m = ROW.match(line)
        if not m:
            continue
        sect, sect_off, sym, addr, flags, obj = m.groups()
        if base is None:
            continue  # rows cannot be converted before the header is seen
        off = int(addr, 16) - base
        if off < 0:
            continue
        at.setdefault(off, []).append(
            (sym, int(sect, 16), int(sect_off, 16), obj, flags.split()))
    return sorted(at.items()), base


def load_map(mapfile):
    """(entries, preferred_base) for an MSVC /MAP.

    entries is [(image_offset, [(symbol, section, sect_off, obj, flags), ...])]
    sorted by image_offset, with every symbol that landed on an address grouped
    into one row so folding is visible rather than hidden.
    """
    if not os.path.isfile(mapfile):
        sys.exit("faultmap: no map at %s -- build first, or pass --map" % mapfile)
    if os.path.getsize(mapfile) < 4096:
        sys.exit("faultmap: map at %s is %d bytes, which is a failed or "
                 "truncated link, not a build to symbolize"
                 % (mapfile, os.path.getsize(mapfile)))
    with open(mapfile, errors="replace") as f:
        entries, base = parse_map(f)
    if base is None:
        sys.exit("faultmap: no 'Preferred load address' header in %s -- that is "
                 "not an MSVC /MAP" % mapfile)
    if not entries:
        sys.exit("faultmap: %s parsed to zero symbol rows" % mapfile)
    return entries, base


def actor_names(root):
    """{actor id: [name, ...]} from the registry's own rows.

    Two files spell rows: hal/actor_classes.inc (the generated bulk) and
    hal/actor_registry.cpp (PLAYER and CAMERA, which are written into the table
    body directly). An id with more than one name is returned with both rather
    than resolved here -- that is a registry defect and should read as one.
    """
    out = {}
    for rel in ("port/hal/actor_classes.inc", "port/hal/actor_registry.cpp"):
        path = os.path.join(root, rel)
        try:
            with open(path, errors="replace") as f:
                text = f.read()
        except OSError:
            continue
        for num, name in ACTOR_ROW.findall(text):
            i = int(num, 16) if num.startswith("0x") else int(num)
            if name not in out.setdefault(i, []):
                out[i].append(name)
    return out


def image_size(mapfile):
    """SizeOfImage from the PE next to the map, or None.

    NEEDED TO REFUSE, NOT TO RESOLVE. A crash dump carries frames from system
    DLLs alongside the module's own -- the forced-quarantine proof run faults
    inside ntdll's RaiseException and prints `at +0x7598a2b4`, with three more
    frames in the same range. Those offsets are relative to walk_window's base
    but land nowhere near it, and a nearest-preceding lookup answers them with
    the LAST symbol in the map and a distance of two gigabytes. That answer is
    confident, specific and wrong, which is the worst thing this tool can be.
    """
    exe = os.path.splitext(mapfile)[0] + ".exe"
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


def lookup(entries, off):
    """(preceding_entry, distance, next_entry) for an image offset."""
    keys = [e[0] for e in entries]
    i = bisect.bisect_right(keys, off) - 1
    if i < 0:
        return None, None, entries[0] if entries else None
    nxt = entries[i + 1] if i + 1 < len(entries) else None
    return entries[i], off - entries[i][0], nxt


FOLD_SHOWN = 6

# A hit more than this past its symbol is not a resolution. Mid-map the
# bisect bounds the distance by the real gap to the next symbol -- 0x1dff0 at
# the widest measured on the linked map -- so this can only fire past the
# LAST symbol, where nothing else bounds it: an ntdll frame with no exe
# beside the map to refuse it by image size lands there with a distance of
# gigabytes. Mirrored in tools/resolve_crash.py MAX_CODE_DELTA -- change both
# together.
MAX_SYM_DELTA = 0x40000


def describe(entries, off, indent="", show_all=False, size=None):
    """Print one offset's resolution."""
    print("%s+0x%08x" % (indent, off))
    if size is not None and off >= size:
        print("%s  OUTSIDE the module: walk_window.exe is 0x%x bytes of image "
              "and this offset is past its end." % (indent, size))
        print("%s  A system DLL frame (the fault probe prints every return "
              "address relative to walk_window's base, including the ones that "
              "are not in it). Not resolvable from this map." % indent)
        return
    prev, dist, nxt = lookup(entries, off)
    if prev is None:
        print("%s  before the first symbol in the map" % indent)
        return
    if dist > MAX_SYM_DELTA:
        print("%s  +0x%x past the nearest preceding symbol (+0x%08x %s): "
              "nothing real is that far past its symbol."
              % (indent, dist, prev[0], prev[1][0][0]))
        print("%s  A system DLL frame or a stack/heap address the probe made "
              "module-relative anyway, most likely with no exe next to the "
              "map to refuse it by image size. Not resolvable." % indent)
        return
    syms = prev[1]
    if len(syms) > 1:
        print("%s  FOLDED, %d symbols share this address:" % (indent, len(syms)))
    shown = syms if (show_all or len(syms) <= FOLD_SHOWN) else syms[:FOLD_SHOWN]
    for sym, sect, sect_off, obj, flags in shown:
        kind = "func" if "f" in flags else "data"
        print("%s  %-4s %s" % (indent, kind, sym))
        print("%s         in %s   (%04x:%08x)" % (indent, obj, sect, sect_off))
    if len(shown) < len(syms):
        print("%s  ... and %d more folded here, --all to list them"
              % (indent, len(syms) - len(shown)))
    print("%s  +0x%x into it (symbol starts at +0x%08x)" % (indent, dist, prev[0]))
    if nxt:
        nname = nxt[1][0][0]
        more = "  (+%d more folded)" % (len(nxt[1]) - 1) if len(nxt[1]) > 1 else ""
        print("%s  next symbol: +0x%08x %s%s  [%s]"
              % (indent, nxt[0], nname, more, nxt[1][0][3]))
    else:
        print("%s  next symbol: none, this is the last symbol in the map" % indent)


def decode_id(names, aid, indent=""):
    got = names.get(aid)
    if not got:
        print("%sactor id 0x%x (%d): NOT IN THE REGISTRY -- no row in "
              "hal/actor_classes.inc or hal/actor_registry.cpp claims it"
              % (indent, aid, aid))
    elif len(got) == 1:
        print("%sactor id 0x%x (%d) = %s" % (indent, aid, aid, got[0]))
    else:
        print("%sactor id 0x%x (%d) = %s  (AMBIGUOUS: the registry has %d rows "
              "for this id)" % (indent, aid, aid, " / ".join(got), len(got)))


# ---------------------------------------------------------------------------
# selftest: fixture map, so the refusal arms are pinned (the style of
# port/tools/abicheck.py --selftest)
# ---------------------------------------------------------------------------
FIX_MAP = """\
 Preferred load address is 00400000

 0001:00001000       _bootstrap                 00401000 f   boot.cpp.obj
 0001:00001100       ?Render@Trap@@QAEHXZ       00401100 f   trap.cpp.obj
 0001:00001100       ?Render@MontyMole@@QAEHXZ  00401100 f   monty.cpp.obj
 0001:00002000       _last_sym                  00402000 f   tail.cpp.obj
"""
# fixture image: symbols end at +0x2000, the image runs out to here
FIX_SIZE = 0x100000


def selftest():
    import io
    from contextlib import redirect_stdout
    bad = 0
    print("faultmap --selftest")
    entries, base = parse_map(FIX_MAP.splitlines())

    def run(off, size):
        buf = io.StringIO()
        with redirect_stdout(buf):
            describe(entries, off, indent="  ", size=size)
        return buf.getvalue()

    def want(note, got, needle, absent=None):
        nonlocal bad
        ok = needle in got and (absent is None or absent not in got)
        bad += 0 if ok else 1
        print("  %-4s %s" % ("ok" if ok else "FAIL", note))
        if not ok:
            print("       got:\n%s" % got)

    ok = base == 0x400000 and len(entries) == 3 \
        and len(dict(entries)[0x1100]) == 2
    bad += 0 if ok else 1
    print("  %-4s fixture parses: 3 addresses, base 0x400000, one folded pair"
          % ("ok" if ok else "FAIL"))

    # normal resolution is untouched, folding stays visible
    want("a folded in-code offset resolves and says FOLDED",
         run(0x1104, FIX_SIZE), "FOLDED, 2 symbols")

    # THE FLAGGED SHAPE, report 7447e46c: an offset past the image end must
    # read OUTSIDE and never name a symbol
    want("past the image end reads OUTSIDE, no symbol named",
         run(0x200000, FIX_SIZE), "OUTSIDE the module", absent="_last_sym")

    # exe missing, size unknown: the distance bound must catch the same
    # offset instead of resolving it to the last symbol +0x1fe000
    want("size unknown: the distance bound alone still refuses",
         run(0x200000, None), "Not resolvable", absent="tail.cpp.obj")

    # and a sane delta past the last symbol stays resolvable without a size,
    # so the bound cannot eat real resolutions when the exe is missing
    want("size unknown: a sane delta still resolves",
         run(0x2010, None), "_last_sym")

    print("SELFTEST %s" % ("PASSED" if not bad else "FAILED (%d)" % bad))
    return 1 if bad else 0


def main():
    args = list(sys.argv[1:])
    if "--selftest" in args:
        return selftest()
    mapfile = None
    if "--map" in args:
        i = args.index("--map")
        mapfile = args[i + 1]
        del args[i:i + 2]
    show_all = "--all" in args
    if show_all:
        args.remove("--all")
    ids = []
    while "--id" in args:
        i = args.index("--id")
        ids.append(int(args[i + 1], 0))
        del args[i:i + 2]
    bad = [a for a in args if a.startswith("--")]
    if bad:
        sys.exit("faultmap: unknown flag(s): %s" % " ".join(bad))

    root = default_root()
    if mapfile is None:
        mapfile = os.path.join(root, "build", "port", "walk_window.map")
    names = actor_names(root)

    if ids and not args:
        # id decode alone needs no map
        for aid in ids:
            decode_id(names, aid)
        return 0

    entries, base = load_map(mapfile)
    size = image_size(mapfile)
    print("map        : %s" % os.path.abspath(mapfile))
    print("image base : 0x%08x (preferred), %d distinct addresses, %d symbols"
          % (base, len(entries), sum(len(e[1]) for e in entries)))
    print("registry   : %d actor ids from hal/actor_classes.inc + "
          "hal/actor_registry.cpp" % len(names))
    print("image size : %s"
          % ("0x%x from the PE next to the map" % size if size
             else "unknown, no readable exe next to the map -- offsets past "
                  "the module are caught only by the +0x%x distance bound"
                  % MAX_SYM_DELTA))
    print()

    for aid in ids:
        decode_id(names, aid)
    if ids:
        print()

    if not args:
        sys.exit("faultmap: give an offset (+0x1234), a crash.txt / playlog "
                 "path, or --id")

    for a in args:
        if os.path.isfile(a):
            print("=== %s ===" % a)
            with open(a, errors="replace") as f:
                lines = f.readlines()
            hits = 0
            for line in lines:
                w = WALKER.search(line)
                if w:
                    print(line.rstrip())
                    decode_id(names, int(w.group(3), 16), indent="  ")
                    print()
                    hits += 1
                    continue
                offs = OFFSET_IN_TEXT.findall(line)
                if not offs:
                    continue
                print(line.rstrip())
                for o in offs:
                    describe(entries, int(o, 16), indent="  ",
                             show_all=show_all, size=size)
                print()
                hits += 1
            if not hits:
                print("  no fault offsets and no walker line in this file")
                print()
        else:
            try:
                off = int(a, 16) if not a.startswith(("+0x", "0x", "+")) \
                    else int(a.lstrip("+"), 16)
            except ValueError:
                sys.exit("faultmap: %r is neither a hex offset nor a file" % a)
            describe(entries, off, show_all=show_all, size=size)
            print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
