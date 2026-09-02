#!/usr/bin/env python3
"""vs_pal16_check.py -- turn a vs_pal16_proof.sh run into a colour verdict.

WHAT IT READS. SM64DS_VS_COLOR_PROBE makes every window print, once per drawn
player per frame, a line of the shape

  [vscol] f12 slot3 char=3 param1=... pal61c=5545 TEXPLTT_BASE=5545 \\
          col=167c,22de,033f,579e tris=298

pal61c is Player+0x61C, the row the resource load picked. TEXPLTT_BASE is the
value read back OUT OF THE MAPPED I/O WINDOW after the draw, so it is the last
palette the engine was actually handed for that player, and col is the first
four entries the decoder will sample at that base -- the body ramp.

WHAT IT ASSERTS, and each of these is a separate way the old bug showed:

  1. EVERY SEAT IS A DIFFERENT COLOUR. The plain form of "player 5 looked like
     a copy of player 1".
  2. THE ROW INDEX IS THE TRUE SLOT. base(slot k) - base(slot 0) == 2*k. One
     step of mPlayerNo is one sixteen-colour row, and the old code stepped by a
     slot that had been cycled 1,2,3, so seat 4 landed back on seat 1's row.
  3. THE LATCH AGREES WITH THE SELECTOR. TEXPLTT_BASE == pal61c. A selector
     that is right and never spent is exactly what the port shipped before the
     material stamp existed.
  4. EVERY WINDOW AGREES ABOUT EVERY SEAT. A colour that is right on your own
     screen and wrong on everyone else's is the online failure, and it is
     invisible to a one-window capture.
  5. A NAMED PICK LANDS. Optionally, --expect slot=w,w,w,w checks one seat's
     four words against a row computed by vs_palette_test --emit, which is the
     shipped arithmetic rather than a number typed here.

  python port/tools/vs_pal16_check.py <out dir> [--expect 5=hex,hex,hex,hex]
"""

import argparse
import os
import re
import sys

LINE = re.compile(
    r"\[vscol\] f(\d+) slot(\d+) .*?pal61c=(-?\d+) TEXPLTT_BASE=(\d+) "
    r"col=([0-9a-f]{4}),([0-9a-f]{4}),([0-9a-f]{4}),([0-9a-f]{4}) tris=(-?\d+)")

FAIL = []


def check(what, cond):
    print("  %s %s" % ("ok  " if cond else "FAIL", what))
    if not cond:
        FAIL.append(what)


def read_window(path):
    """slot -> (pal61c, base, colour quad, frames seen, frames with triangles)"""
    seats = {}
    with open(path, "r", errors="replace") as f:
        for ln in f:
            m = LINE.search(ln)
            if not m:
                continue
            slot = int(m.group(2))
            pal = int(m.group(3))
            base = int(m.group(4))
            col = tuple(m.group(i) for i in range(5, 9))
            tris = int(m.group(9))
            e = seats.setdefault(slot, {"pal": set(), "base": set(),
                                        "col": set(), "n": 0, "drawn": 0})
            e["pal"].add(pal)
            e["base"].add(base)
            e["col"].add(col)
            e["n"] += 1
            if tris > 0:
                e["drawn"] += 1
    return seats


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("out")
    ap.add_argument("--expect", action="append", default=[],
                    help="slot=w,w,w,w -- one seat's expected body ramp")
    a = ap.parse_args()

    wins = {}
    for name in sorted(os.listdir(a.out)):
        log = os.path.join(a.out, name, "run.log")
        if name.startswith("p") and os.path.isfile(log):
            wins[name] = read_window(log)
    if not wins:
        print("no p*/run.log under %s" % a.out)
        return 2

    print("windows: %s" % ", ".join(sorted(wins)))
    for w in sorted(wins):
        seats = wins[w]
        print("  %s sees %d seats: %s" %
              (w, len(seats), ", ".join(
                  "%d=%s" % (s, "/".join(sorted(seats[s]["col"])[0]))
                  for s in sorted(seats))))

    ref_name = sorted(wins)[0]
    ref = wins[ref_name]
    slots = sorted(ref)

    print("\n1. every seat wears a different body ramp")
    cols = {}
    for s in slots:
        c = sorted(ref[s]["col"])
        cols[s] = c[0]
        check("seat %d never changed colour mid-run" % s, len(c) == 1)
    dupes = []
    for i, s in enumerate(slots):
        for t in slots[i + 1:]:
            if cols[s] == cols[t]:
                dupes.append((s, t))
    check("no two seats share a ramp: %s" % (dupes or "-"), not dupes)

    print("\n2. the row index is the TRUE slot")
    bases = {}
    for s in slots:
        b = sorted(ref[s]["base"])
        check("seat %d has one stable palette base" % s, len(b) == 1)
        bases[s] = b[0] if b else None
    if 0 in bases and bases[0] is not None:
        bad = [(s, bases[s] - bases[0], 2 * s) for s in slots
               if bases[s] is not None and bases[s] - bases[0] != 2 * s]
        check("base(slot k) - base(slot 0) == 2k for every seat: %s"
              % (bad or "-"), not bad)
    else:
        check("seat 0 was drawn at all", False)

    print("\n3. the latch agrees with the selector, and geometry was drawn")
    for s in slots:
        p = sorted(ref[s]["pal"])
        b = sorted(ref[s]["base"])
        check("seat %d TEXPLTT_BASE == Player+0x61C (%s vs %s)"
              % (s, b, p), p == b)
    nodraw = [s for s in slots if ref[s]["drawn"] == 0]
    check("every seat submitted triangles behind its palette: %s"
          % (nodraw or "-"), not nodraw)

    print("\n4. every window agrees about every seat")
    for w in sorted(wins):
        if w == ref_name:
            continue
        seats = wins[w]
        missing = [s for s in slots if s not in seats]
        check("%s drew all %d seats: missing %s" % (w, len(slots),
                                                    missing or "-"),
              not missing)
        wrong = [(s, sorted(seats[s]["col"])[0], cols[s])
                 for s in slots if s in seats
                 and sorted(seats[s]["col"])[0] != cols[s]]
        check("%s paints every seat the same colour %s does: %s"
              % (w, ref_name, wrong or "-"), not wrong)

    if a.expect:
        print("\n5. a named pick lands on its own seat")
        for e in a.expect:
            slot, _, want = e.partition("=")
            slot = int(slot)
            want = tuple(w.strip().lower() for w in want.split(",")[:4])
            got = cols.get(slot)
            check("seat %d wears %s (got %s)"
                  % (slot, ",".join(want), ",".join(got) if got else "nothing"),
                  got == want)

    print("\n%s" % ("FAILED: " + "; ".join(FAIL) if FAIL else "all green"))
    return 1 if FAIL else 0


if __name__ == "__main__":
    sys.exit(main())
