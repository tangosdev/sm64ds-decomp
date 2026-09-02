#!/usr/bin/env python3
"""vs_pal16_census.py -- count, per window, how many pixels each SEAT painted.

NOT A JUDGEMENT OF THE PICTURE. This is status/VSCOL.md's palcensus.py grown
from two rows to sixteen. It is a mechanical count against the exact BGR555
entries the running game reported binding for each seat
(SM64DS_VS_COLOR_PROBE's `col=` quad), expanded the way the port's raster
expands them ((c << 3) | (c >> 2)). It says whether each seat's own colours
actually reached the framebuffer, and nothing whatever about whether the frame
looks right. That is the owner's call, on the BMPs themselves.

WHY IT IS WORTH RUNNING AT ALL when vs_pal16_check.py has already read the
palette back out of the I/O window: the check proves the ENGINE was handed the
right palette, and this proves PIXELS came out of it. The whole 0.3.2 defect
lived in the gap between those two -- the selector was right the entire time
and nothing spent it.

ONE HONEST LIMIT. Two seats whose ramps share a word are counted for both at
that word; the per-word breakdown is printed so a shared word is visible rather
than hidden. Seats that picked pure black (0000) are excluded from the shared
count by default, because 0000 is also the background and the outline in a
great many places -- pass --include-black to count it anyway and read the
number knowing that.

  python port/tools/vs_pal16_census.py <out dir> [--include-black]
"""
import argparse
import os
import re
import struct
import sys

LINE = re.compile(
    r"\[vscol\] f(\d+) slot(\d+) .*?TEXPLTT_BASE=(\d+) "
    r"col=([0-9a-f]{4}),([0-9a-f]{4}),([0-9a-f]{4}),([0-9a-f]{4})")


def rgb(v):
    r, g, b = v & 31, (v >> 5) & 31, (v >> 10) & 31
    return ((r << 3) | (r >> 2), (g << 3) | (g >> 2), (b << 3) | (b >> 2))


def read_bmp(path):
    d = open(path, 'rb').read()
    off = struct.unpack_from('<I', d, 10)[0]
    w, h = struct.unpack_from('<ii', d, 18)
    bpp = struct.unpack_from('<H', d, 28)[0]
    return d, off, w, abs(h), bpp


def seats_from_log(path):
    """slot -> the four body words the game said it bound."""
    seats = {}
    with open(path, 'r', errors='replace') as f:
        for ln in f:
            m = LINE.search(ln)
            if m:
                seats.setdefault(int(m.group(2)),
                                 tuple(int(m.group(i), 16) for i in range(4, 8)))
    return seats


def census(bmp, seats, include_black):
    d, off, w, h, bpp = read_bmp(bmp)
    px = bpp // 8
    stride = ((w * px + 3) // 4) * 4
    want = {}
    for slot, quad in sorted(seats.items()):
        for i, v in enumerate(quad):
            if v == 0 and not include_black:
                continue
            want.setdefault(rgb(v), []).append((slot, i))
    per = {}
    for y in range(h):
        base = off + y * stride
        row = d[base:base + w * px]
        for x in range(w):
            k = want.get((row[x * px + 2], row[x * px + 1], row[x * px]))
            if k:
                for key in k:
                    per[key] = per.get(key, 0) + 1
    return w, h, bpp, per


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("--include-black", action="store_true")
    a = ap.parse_args()

    any_bmp = False
    for name in sorted(os.listdir(a.out)):
        wdir = os.path.join(a.out, name)
        log = os.path.join(wdir, "run.log")
        if not (name.startswith("p") and os.path.isfile(log)):
            continue
        bmp = os.path.join(wdir, "window_%s.bmp" % name)
        if not os.path.isfile(bmp):
            print("%s: no window_%s.bmp" % (name, name))
            continue
        any_bmp = True
        seats = seats_from_log(log)
        w, h, bpp, per = census(bmp, seats, a.include_black)
        print("\n%s  %s  %dx%d %dbpp  %d seats reported" %
              (name, os.path.basename(bmp), w, h, bpp, len(seats)))
        for slot in sorted(seats):
            quad = seats[slot]
            tot = sum(per.get((slot, i), 0) for i in range(4))
            words = " ".join("%04x:%d" % (quad[i], per.get((slot, i), 0))
                             for i in range(4))
            allblack = all(v == 0 for v in quad)
            note = "  (pure black; words excluded unless --include-black)" \
                if allblack and not a.include_black else ""
            print("   seat %-2d total %6d   %s%s" % (slot, tot, words, note))
        painted = [s for s in sorted(seats)
                   if sum(per.get((s, i), 0) for i in range(4)) > 0]
        print("   seats with pixels on screen: %s" % (painted or "none"))
    if not any_bmp:
        print("no window BMPs under %s" % a.out)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
