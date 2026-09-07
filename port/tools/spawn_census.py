#!/usr/bin/env python3
"""spawn_census -- the whole-cartridge actor placement census, and which ids
the port's registry actually routes.

This is ov_places.py widened two ways, because a seat worklist needs both:

  ALL STAR GROUPS.  ov_places.py filters each object sub-table entry to
  `group == 0 or group == --star`, which is the ROM's own reading for one
  mission.  A worklist wants "does the cartridge ever name this id", so the
  sweep here runs every group 0..7 and reports the union alongside the
  star-1 reading, so a class that only appears under star 3 is not invisible.

  THE REGISTRY JOIN.  hal/actor_classes.inc's rows plus the two written
  straight into hal/actor_registry.cpp (PLAYER 0x0bf, CAMERA 0x14c) are the
  set of ids the spawn gate lets through.  Everything else the pre-spawn hook
  names once and declines.

Decoding is ov_places.py's, unchanged and for the same reasons: LVL_Overlay
misc table at +4, sub-table array at +0x10 with the count at +0x14, 8-byte
entries {descriptor, count, pad2, records}, descriptor low five bits the
sub-loader index and top three the star group; strides 0x10 for Standard (0)
and Entrance (1), 8 for Simple (5), 0xC for Door (9); a raw index becomes an
actor id through ov002's data_ov002_0210cbf4, and a Door record's id comes
from data_ov002_0210cb88[fielda & 0x1f].

    python port/tools/spawn_census.py <root>              tsv to stdout
    python port/tools/spawn_census.py <root> --summary     counts only
"""
import argparse
import collections
import pathlib
import re
import struct
import sys

ARM9_BASE = 0x02004000
LEVEL_OV = 0x020758C8
LEVEL_LVLOV = 0x02092208
N_LEVELS = 52
RAW_TO_ACTOR = 0x0210CBF4
DOOR_IDS = 0x0210CB88
TABLE = 0x02090864
N_ACTORS = 391


def ranges(root):
    out = {}
    for p in sorted((root / "config/arm9/overlays").glob("ov*/delinks.txt")):
        ov = int(re.search(r"ov(\d+)", str(p)).group(1))
        t = p.read_text()
        s = [int(x, 16) for x in re.findall(r"start:(0x[0-9a-fA-F]+)", t)]
        e = [int(x, 16) for x in re.findall(r"end:(0x[0-9a-fA-F]+)", t)]
        out[ov] = (min(s), max(e))
    return out


class Image(object):
    def __init__(self, root, ovid, rng):
        self.base = rng[ovid][0]
        self.d = (root / ("extracted/overlays/overlay_%04d.bin" % ovid)).read_bytes()

    def has(self, a, n=1):
        return self.base <= a and a + n <= self.base + len(self.d)

    def u8(self, a):
        return self.d[a - self.base]

    def u16(self, a):
        return struct.unpack_from("<H", self.d, a - self.base)[0]

    def u32(self, a):
        return struct.unpack_from("<I", self.d, a - self.base)[0]


def registry_ids(root):
    """ids the spawn gate routes: the .inc rows plus the two literal rows."""
    inc = (root / "port/hal/actor_classes.inc").read_text()
    body = inc.split("#define PORT_ACTOR_CLASS_ROWS", 1)[1]
    rows = re.findall(r'\{\s*(\d+)\s*,\s*"([A-Za-z0-9_]+)"', body)
    names = {int(a): b for a, b in rows}
    reg = (root / "port/hal/actor_registry.cpp").read_text()
    for a, b in re.findall(r'\{\s*(0x[0-9a-f]+)\s*,\s*"([A-Za-z0-9_]+)"', reg):
        names[int(a, 16)] = b
    return names


def sweep(root, rng, arm9):
    """per (id, level) counts, split star-1 vs any-group."""
    def a32(x):
        return struct.unpack_from("<I", arm9, x - ARM9_BASE)[0]

    ov2 = Image(root, 2, rng)
    raw2act = []
    i = 0
    while ov2.has(RAW_TO_ACTOR + i * 2, 2) and i < 512:
        raw2act.append(ov2.u16(RAW_TO_ACTOR + i * 2))
        i += 1
    door_ids = [ov2.u16(DOOR_IDS + 2 * i) for i in range(32)]

    star1 = collections.defaultdict(collections.Counter)
    anyst = collections.defaultdict(collections.Counter)
    groups = collections.defaultdict(set)
    undec = []
    for lvl in range(N_LEVELS):
        ovid = a32(LEVEL_OV + lvl * 4)
        lo = a32(LEVEL_LVLOV + lvl * 4)
        if ovid not in rng:
            undec.append((lvl, ovid, lo, "no such overlay"))
            continue
        img = Image(root, ovid, rng)
        if not img.has(lo, 0x18):
            undec.append((lvl, ovid, lo, "LVL_Overlay outside the image"))
            continue
        tables = [img.u32(lo + 4)]
        subs = img.u32(lo + 0x10)
        nsub = img.u8(lo + 0x14)
        if img.has(subs, nsub * 0xC):
            for s in range(nsub):
                tables.append(img.u32(subs + s * 0xC))
        ok = False
        for t in tables:
            if not t or not img.has(t, 8):
                continue
            n = img.u16(t)
            ents = img.u32(t + 4)
            if not img.has(ents, n * 8):
                continue
            ok = True
            for j in range(n):
                e = ents + j * 8
                desc = img.u8(e)
                grp, idx = (desc >> 5) & 7, desc & 0x1F
                cnt = img.u8(e + 1)
                recs = img.u32(e + 4)
                stride = {0: 0x10, 1: 0x10, 5: 8, 9: 0xC}.get(idx)
                if stride is None or not img.has(recs, cnt * stride):
                    continue
                for k in range(cnt):
                    r = recs + k * stride
                    if idx == 9:
                        di = img.u16(r + 0xA) & 0x1F
                        aid = door_ids[di]
                    else:
                        raw = img.u16(r)
                        if raw >= len(raw2act):
                            continue
                        aid = raw2act[raw]
                    anyst[aid][lvl] += 1
                    groups[aid].add(grp)
                    if grp == 0 or grp == 1:
                        star1[aid][lvl] += 1
        if not ok:
            undec.append((lvl, ovid, lo, "no decodable table"))
    return star1, anyst, groups, undec


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--summary", action="store_true")
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    rng = ranges(root)
    arm9 = (root / "extracted/arm9_dec.bin").read_bytes()
    names = registry_ids(root)
    star1, anyst, groups, undec = sweep(root, rng, arm9)

    if a.summary:
        placed = sorted(anyst)
        unh = [i for i in placed if i not in names]
        print("levels swept        : %d of %d (%d undecodable: %s)"
              % (N_LEVELS - len(undec), N_LEVELS, len(undec),
                 [u[0] for u in undec]))
        print("distinct ids placed : %d" % len(placed))
        print("  hosted            : %d" % (len(placed) - len(unh)))
        print("  UNHOSTED          : %d  %s" % (len(unh), unh))
        print("total placements    : %d (any star), %d (star 1)"
              % (sum(sum(c.values()) for c in anyst.values()),
                 sum(sum(c.values()) for c in star1.values())))
        print("registry rows       : %d" % len(names))
        print("registered but never placed : %d"
              % len([i for i in names if i not in anyst]))
        return

    print("id\tname\thosted\tappearances_any\tappearances_star1\tn_levels\tstar_groups\tlevels")
    for aid in sorted(set(list(anyst) + list(names))):
        c = anyst.get(aid, collections.Counter())
        lv = sorted(c)
        print("%d\t%s\t%s\t%d\t%d\t%d\t%s\t%s"
              % (aid, names.get(aid, "?"), "yes" if aid in names else "NO",
                 sum(c.values()), sum(star1.get(aid, {}).values()), len(lv),
                 ",".join(str(g) for g in sorted(groups.get(aid, []))) or "-",
                 ",".join(str(x) for x in lv) or "-"))


if __name__ == "__main__":
    main()
