#!/usr/bin/env python3
"""Which SOURCE produced each object in the map, and where that makes the
linkage headline wrong.

THE BUG THIS WAS WRITTEN TO MEASURE, AND WHY IT STILL RUNS AT ZERO. An MSVC
/MAP names an object by its BASENAME only: `func_02043fdc.cpp.obj`. CMake keeps
the two files apart on disk (`CMakeFiles/walk_window.dir/C_/tmp/cl1/src/...`
versus `CMakeFiles/walk_window.dir/unmatched/...`) but the map keeps no trace of
that, so when src/ and port/unmatched/ both carry a file of the same name the
map cannot tell you which one is in the binary.

linkage.py counts a matched TU as LINKED when an object of its stem appears in
the map. For `func_02043fdc` the object in the map was port/unmatched's HOST
COPY -- a hand-written stand-in that resolves the mwcc pointer-to-member
dispatch MSVC cannot express -- and src/func_02043fdc.cpp was not in the binary
at all. The headline counted it as decompiled code that reaches the game. It is
the opposite: it is scaffolding, and it was scaffolding the replacement queue
also could not see, because that queue decides "is this a host object" by asking
whether the object's stem collides with a src/ TU, which for these files is
exactly backwards.

So the number was inflated and the work list was short by the same items. Both
halves came from the same missing fact, and the fact is recoverable:
build.ninja records the real source path of every object, and it is generated
from the same CMakeLists the link uses.

THE EIGHTEEN THIS FOUND ARE FIXED, AND HOST 0 IS THE EXPECTED READING. At the
wave C close every one of them was renamed to `<stem>_hostcopy`, so no host
object's stem collides with a matched TU and the headline dropped by exactly
eighteen to match. A HOST count of zero is therefore the normal result now, and
it is not a reason to retire this check: the renames make the collision absent
today, and this is what says so tomorrow. It asks the BUILD which source
produced each object rather than trusting the naming convention, so the next
host copy someone names after its matched TU is caught the same way these were.
A nonzero HOST count means that has happened again; read the rows, rename, and
expect the headline to fall by that many.

    python port/tools/objsrc_check.py [repo-root] [--all]

Prints every object in walk_window.map whose source is NOT the src/ TU of the
same name, split into the two cases that matter:

  HOSTGEN   the source is build/host-src/src/<stem>.cpp, hostgen's rewrite of
            the matched TU. Counting it as linked is CORRECT -- it IS the
            matched code, transformed for the host by a tool whose transform is
            reviewable. Listed so the count is explained rather than assumed.
  HOST      the source is under port/hal or port/unmatched. Counting it as
            linked is WRONG. Each one is an over-count of the headline and a
            missing row in the replacement queue.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import linkage  # noqa: E402


def default_root():
    here = os.path.dirname(os.path.abspath(__file__))
    return os.path.dirname(os.path.dirname(here))


# `build CMakeFiles\<target>.dir\<path>.obj: <rule> C$:\...\file.cpp || ...`
# ninja escapes a colon in a path as `$:`, and CMake writes Windows separators.
BUILD_RULE = re.compile(
    r"^build CMakeFiles\\(\w+)\.dir\\([^:]+?\.obj): \S+ (\S+)",
    re.MULTILINE)

# The targets that make up the walk_window link: its own objects, plus the ntr
# static library it links against. Scoping to these two matters -- the tree
# builds twenty-odd smoke harnesses out of overlapping source sets, and a
# basename read across all of them collides constantly for reasons that say
# nothing about the binary under test. ntr_wide_rt is the one the walk_window
# target links (ntr and ntr_hires are the 1x and 1024x768 tiers); it is the
# runtime-selectable 16:9 layer that replaced the fixed ntr_2x tier.
LINK_TARGETS = ("walk_window", "ntr_wide_rt")


def object_sources(root, targets=LINK_TARGETS):
    """{object basename: set(source path)} for the targets in this link."""
    ninja = os.path.join(root, "build", "port", "build.ninja")
    if not os.path.isfile(ninja):
        sys.exit("objsrc: no build.ninja at %s -- configure the port first"
                 % ninja)
    with open(ninja, errors="replace") as f:
        text = f.read()
    out = {}
    for target, obj, src in BUILD_RULE.findall(text):
        if target not in targets:
            continue
        base = obj.split("\\")[-1]
        src = src.replace("$:", ":").replace("\\", "/")
        out.setdefault(base, set()).add(src)
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    show_all = "--all" in sys.argv
    root = os.path.abspath(args[0]) if args else default_root()
    mapfile = os.path.join(root, "build", "port", "walk_window.map")

    print("repo root : %s" % root)
    print("map       : %s" % mapfile)
    print()

    matched = linkage.matched_index(root)
    objsrc = object_sources(root)
    print("walk_window objects in build.ninja: %d" % len(objsrc))

    mapobjs = set()
    for _sym, obj in linkage.map_symbols(mapfile):
        mapobjs.add(obj.split(":")[-1])
    print("objects named in the map          : %d" % len(mapobjs))
    print()

    hostgen, host, missing, collide = [], [], [], []
    for obj in sorted(mapobjs):
        stem = os.path.splitext(os.path.splitext(obj)[0])[0]
        if stem not in matched:
            continue
        srcs = objsrc.get(obj)
        if not srcs:
            missing.append(obj)
            continue
        if len(srcs) > 1:
            collide.append((obj, sorted(srcs)))
            continue
        src = next(iter(srcs))
        want = (root + "/" + matched[stem]).replace("\\", "/")
        if os.path.normcase(src) == os.path.normcase(want):
            continue
        nsrc = src.replace("\\", "/")
        if "/build/port/host-src/" in nsrc:
            hostgen.append((obj, src))
        elif nsrc.startswith((root + "/src/").replace("\\", "/")):
            # STILL THE MATCHED TU, just not the one matched_index picked.
            # src/ carries both a .c and a .cpp for a handful of stems and the
            # index keeps whichever it walked first, so the paths differ while
            # the file is decompiled code either way. Calling that a host
            # stand-in would be the same over-claim in the other direction.
            continue
        else:
            host.append((obj, src, matched[stem]))

    print("Objects the headline counts as a linked matched TU, whose source is")
    print("not that TU:")
    print("  %-8s %5d  hostgen rewrite of the matched TU -- correct to count"
          % ("HOSTGEN", len(hostgen)))
    print("  %-8s %5d  a HOST stand-in -- counted as linked, and it is not"
          % ("HOST", len(host)))
    if collide:
        print("  %-8s %5d  one object basename, several sources in one target"
              % ("COLLIDE", len(collide)))
    if missing:
        print("  %-8s %5d  in the map, no rule in build.ninja (a library "
              "object)" % ("NORULE", len(missing)))
    print()
    if not host:
        print("HOST (0): no host object's stem collides with a matched TU, so")
        print("the linkage headline is not over-counting on this axis. This is")
        print("the expected reading since the wave C close renamed the eighteen")
        print("that did to <stem>_hostcopy. It is a check that passed, not a")
        print("check with nothing left to do -- a new host copy named after its")
        print("matched TU would show up here as a row and as a headline that")
        print("rose without a seat behind it.")
    else:
        print("HOST (%d): each row is one over-count of the linkage headline and"
              % len(host))
        print("one row the replacement queue does not print. Rename each to")
        print("<stem>_hostcopy and expect the headline to fall by %d."
              % len(host))
    for obj, src, tu in host:
        print("  %-30s %s" % (obj, src.split("/port/", 1)[-1]))
        print("  %-30s   stands in for %s" % ("", tu))
    if collide:
        print()
        print("COLLIDE (%d): the map cannot say which of these is in the "
              "binary." % len(collide))
        for obj, srcs in collide:
            print("  %s" % obj)
            for s in srcs:
                print("      %s" % s)
    if show_all and hostgen:
        print()
        print("HOSTGEN (%d):" % len(hostgen))
        for obj, src in hostgen:
            print("  %-40s %s" % (obj, src.split("/host-src/", 1)[-1]))


if __name__ == "__main__":
    main()
