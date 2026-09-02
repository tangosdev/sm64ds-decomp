#!/usr/bin/env python3
"""What a VS arena PLACES, read off the ROM's own level data.

The port's runtime census says what spawned. This says what the cartridge asked
for, so the two can be differenced instead of argued about. Nothing here is a
guess: every address is read out of extracted/, and the filter is the ROM's own
line.

THE FILTER, verbatim (src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.c:8-15):

    unsigned char b = *e;
    int type = (b >> 5) & 7;
    if (type == 0 || type == data_0209f220) {
        h = data_ov002_0210cbb8[b & 0x1f];
        if (h != 0) h(e, a1, a2);
    }

The descriptor byte at the head of each 8-byte sub-table entry carries the
sub-loader index in bits 0..4 and the STAR GROUP in bits 5..7, and a whole
sub-table either runs or does not. There is no per-record mask.

data_0209f220 IS 2 IN A MATCH, and that is a read rather than a convention:
src/func_ov075_02116c8c.c:11 calls LoadLevelNoReturn(level, 0, 2, 0);
src/LoadLevel.c:30-34 puts that 2 in data_0209f1f0; src/SetNextStar.c:11-13
would force it back to 1 except that data_0209f2d8 == 1 returns first; and
src/_ZN5Stage13InitResourcesEv.cpp:210 copies it into data_0209f220. The same 2
is what arms the VS sound-group branch at InitResources:316.

An actor id is resolved the way the loaders resolve it -- the record holds a RAW
index and Actor::Spawn is called with data_ov002_0210cbf4[raw]
(src/_Z19LoadStandardObjects...cpp:42, src/_Z17LoadSimpleObjects...cpp:44) --
and each id is attributed to the overlay its SpawnInfo lives in, off the arm9's
ACTOR_SPAWN_TABLE at 0x02090864. That last column is the one that matters for a
match, because src/_Z26LoadOrUnloadObjectOverlaysPFviEi.cpp:12-14 returns before
loading any object overlay when data_0209f2d8 == 1.

    python port/tools/vs_objcensus.py            the four arenas at filter 2
    python port/tools/vs_objcensus.py --star 1   the same levels as adventure
    python port/tools/vs_objcensus.py --records  every placed record, verbatim
"""
import argparse
import collections
import glob
import os
import re
import struct

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

# (map number, level id, overlay id, LVL_Overlay DS address). The level list is
# the ROM's own data_ov075_0211c6ec; the LVL_Overlay addresses are
# data_02092208[level], the same words port/hal/level_boot.cpp's mount rows carry
# and port_level_mounts_install asserts against the ROM at boot.
ARENAS = [
    (1, 51, 59, 0x02111548),
    (2, 43, 51, 0x0211150c),
    (3, 29, 37, 0x021111ec),
    (4, 42, 50, 0x02111204),
]

# data_ov002_0210cbb8, the sub-loader dispatch table LoadObjects indexes.
LOADERS = {
    0: "Standard", 1: "Entrance", 2: "PathNode", 3: "Path", 4: "View",
    5: "Simple", 6: "TeleportSrc", 7: "TeleportDst", 8: "Fog", 9: "Door",
    10: "Exit", 11: "MinimapTile", 12: "MinimapScale", 13: "UnusedType13",
    14: "StarCamera",
}

ARM9_BASE = 0x02004000          # extracted/arm9_dec.bin's load address
ACTOR_SPAWN_TABLE = 0x02090864  # 391 SpawnInfo*, tools/actor_names.py's anchor
RAW_TO_ACTOR = 0x0210cbf4       # data_ov002_0210cbf4


def overlay_ranges():
    out = {}
    pat = os.path.join(ROOT, "config", "arm9", "overlays", "ov*", "delinks.txt")
    for p in sorted(glob.glob(pat)):
        ov = int(re.search(r"ov(\d+)", p).group(1))
        t = open(p).read()
        s = [int(x, 16) for x in re.findall(r"start:(0x[0-9a-fA-F]+)", t)]
        e = [int(x, 16) for x in re.findall(r"end:(0x[0-9a-fA-F]+)", t)]
        out[ov] = (min(s), max(e))
    return out


class Image(object):
    """One overlay image, addressed by its DS addresses."""

    def __init__(self, ovid, ranges):
        self.base = ranges[ovid][0]
        p = os.path.join(ROOT, "extracted", "overlays", "overlay_%04d.bin" % ovid)
        self.d = open(p, "rb").read()

    def has(self, a, n=1):
        return self.base <= a and a + n <= self.base + len(self.d)

    def _r(self, a, n):
        return self.d[a - self.base:a - self.base + n]

    def u8(self, a):
        return self._r(a, 1)[0]

    def u16(self, a):
        return struct.unpack("<H", self._r(a, 2))[0]

    def s16(self, a):
        return struct.unpack("<h", self._r(a, 2))[0]

    def u32(self, a):
        return struct.unpack("<I", self._r(a, 4))[0]


def registered_names():
    """id -> NAME for the classes this build registers (hal/actor_classes.inc)."""
    out = {}
    p = os.path.join(ROOT, "port", "hal", "actor_classes.inc")
    for ln in open(p, errors="ignore"):
        m = re.match(r'\s*\{\s*(\d+)\s*,\s*"([A-Z0-9_]+)"', ln)
        if m:
            out[int(m.group(1))] = m.group(2)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--star", type=int, default=2,
                    help="data_0209f220; a match is 2, adventure is 1")
    ap.add_argument("--records", action="store_true",
                    help="print every placed record, not just the roll-up")
    args = ap.parse_args()

    ranges = overlay_ranges()
    arm9 = open(os.path.join(ROOT, "extracted", "arm9_dec.bin"), "rb").read()

    def a32(a):
        return struct.unpack("<I", arm9[a - ARM9_BASE:a - ARM9_BASE + 4])[0]

    spawn_info = [a32(ACTOR_SPAWN_TABLE + i * 4) for i in range(391)]

    def home(aid):
        if not 0 <= aid < len(spawn_info):
            return "?"
        p = spawn_info[aid]
        for ov, (s, e) in ranges.items():
            if s <= p < e:
                return "ov%03d" % ov
        return "arm9"

    ov2 = Image(2, ranges)
    raw_to_actor = []
    i = 0
    while ov2.has(RAW_TO_ACTOR + i * 2, 2):
        raw_to_actor.append(ov2.u16(RAW_TO_ACTOR + i * 2))
        i += 1

    names = registered_names()

    def label(aid):
        return names.get(aid, "(not registered here)")

    print("star filter %d  (a VS match is 2)" % args.star)
    for mapno, lvl, ovid, lo in ARENAS:
        img = Image(ovid, ranges)
        print("")
        print("=" * 76)
        print("VS map %d   level %d   ov%03d   LVL_Overlay 0x%08x"
              % (mapno, lvl, ovid, lo))
        tables = [("misc", img.u32(lo + 4))]
        subs = img.u32(lo + 0x10)
        for s in range(img.u8(lo + 0x14)):
            tables.append(("area%d" % s, img.u32(subs + s * 0xC)))

        placed = collections.Counter()
        filtered = collections.Counter()
        for tag, t in tables:
            if not t:
                print("  %-6s null" % tag)
                continue
            n = img.u16(t)
            ents = img.u32(t + 4)
            for i in range(n):
                e = ents + i * 8
                kind = img.u8(e)
                grp = (kind >> 5) & 7
                idx = kind & 0x1F
                count = img.u8(e + 1)
                recs = img.u32(e + 4)
                run = (grp == 0 or grp == args.star)
                print("  %-6s [%2d] %-12s group %d  count %3d  %s"
                      % (tag, i, LOADERS.get(idx, "idx%d" % idx), grp, count,
                         "RUNS" if run else "skipped"))
                stride = {0: 0x10, 1: 0x10, 5: 8}.get(idx)
                if stride is None or not img.has(recs, count * stride):
                    continue
                for j in range(count):
                    r = recs + j * stride
                    if idx == 1:
                        if args.records:
                            print("          %3d  entrance (%6d,%6d,%6d) "
                                  "yaw %6d param 0x%04x"
                                  % (j, img.s16(r + 2), img.s16(r + 4),
                                     img.s16(r + 6), img.s16(r + 0xA),
                                     img.u16(r + 0xE)))
                        continue
                    raw = img.u16(r) if idx == 0 else (img.u16(r) & 0x1FF)
                    aid = raw_to_actor[raw] if raw < len(raw_to_actor) else -1
                    (placed if run else filtered)[aid] += 1
                    if not args.records:
                        continue
                    if idx == 0:
                        print("          %3d  raw %3d -> %3d %-24s "
                              "(%6d,%6d,%6d) rot(%6d,%6d,%6d) param 0x%04x"
                              % (j, raw, aid, label(aid), img.s16(r + 2),
                                 img.s16(r + 4), img.s16(r + 6), img.s16(r + 8),
                                 img.s16(r + 0xA), img.s16(r + 0xC),
                                 img.u16(r + 0xE)))
                    else:
                        print("          %3d  raw %3d -> %3d %-24s "
                              "(%6d,%6d,%6d) param %d"
                              % (j, raw, aid, label(aid), img.s16(r + 2),
                                 img.s16(r + 4), img.s16(r + 6),
                                 img.u16(r) >> 9))

        print("  PLACED under filter %d:" % args.star)
        for aid, c in sorted(placed.items()):
            print("      %3d 0x%03x  x%-3d %-28s %s"
                  % (aid, aid, c, label(aid), home(aid)))
        if filtered:
            print("  FILTERED OUT (another star group):")
            for aid, c in sorted(filtered.items()):
                print("      %3d 0x%03x  x%-3d %-28s %s"
                      % (aid, aid, c, label(aid), home(aid)))


main()
