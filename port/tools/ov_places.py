#!/usr/bin/env python3
"""ov_places -- which LEVELS place a given actor id, read off the ROM's own
level object tables.

The port's boot census says what a level spawned; this says what the cartridge
asked for, which is the half a seat lane needs BEFORE it registers anything: a
class nobody places has no boot proof available, and a class placed on one
level only has exactly one.

    data_020758c8[level]   the level's object overlay id      (u32 rows --
                           [4]=12, [5]=13, [8]=16, the three
                           hal/level_boot.cpp quotes in its own notes)
    data_02092208[level]   the level's LVL_Overlay address    (u32 rows)

and the LVL_Overlay is decoded the way port/tools/vs_objcensus.py decodes it
(misc table at +4, sub-table array at +0x10 with the count at +0x14; each
8-byte entry is {descriptor, count, pad2, records}, the descriptor's low five
bits the sub-loader index and its top three the star group).  Standard (idx 0)
and Entrance (idx 1) records are 0x10 bytes with the RAW object index in the
first halfword; Simple (idx 5) records are 8.  A raw index becomes an actor id
through ov002's data_ov002_0210cbf4, the same indirection
LoadStandardObjects/LoadSimpleObjects use.

DOORS ARE A SECOND INDIRECTION and a seat lane that misses it reads a whole
family of ids as "placed on no level".  src/_Z15LoadDoorObjectsRN11LVL_Overlay
11ObjSubTableEij.cpp walks 12-byte DoorEntry records under sub-loader index 9
and spawns data_ov002_0210cb88[record.fielda & 0x1f] -- a table whose first
entry is id 348.  Nothing in the Standard/Simple path ever names 348, so the
door table is decoded here too.

Eight of the fifty-two rows point into data-only overlays whose word does not
decode as an LVL_Overlay at all; those are reported as undecodable rather than
guessed at, exactly as hal/actor_registry.cpp's own sweep note records.

    python port/tools/ov_places.py <root> --id 348
    python port/tools/ov_places.py <root> --unregistered
"""
import argparse
import collections
import pathlib
import re
import struct

ARM9_BASE = 0x02004000
LEVEL_OV = 0x020758C8
LEVEL_LVLOV = 0x02092208
N_LEVELS = 52
RAW_TO_ACTOR = 0x0210CBF4
DOOR_IDS = 0x0210CB88          # data_ov002_0210cb88, LoadDoorObjects' id table


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


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root")
    ap.add_argument("--id", type=lambda s: int(s, 0), default=None)
    ap.add_argument("--star", type=int, default=1)
    a = ap.parse_args()
    root = pathlib.Path(a.root).resolve()
    rng = ranges(root)
    arm9 = (root / "extracted/arm9_dec.bin").read_bytes()

    def a32(x):
        return struct.unpack_from("<I", arm9, x - ARM9_BASE)[0]

    ov2 = Image(root, 2, rng)
    raw2act = []
    i = 0
    while ov2.has(RAW_TO_ACTOR + i * 2, 2) and i < 512:
        raw2act.append(ov2.u16(RAW_TO_ACTOR + i * 2))
        i += 1
    door_ids = [ov2.u16(DOOR_IDS + 2 * i) for i in range(32)]

    per_id = collections.defaultdict(collections.Counter)
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
                if not (grp == 0 or grp == a.star):
                    continue
                stride = {0: 0x10, 1: 0x10, 5: 8, 9: 0xC}.get(idx)
                if stride is None or not img.has(recs, cnt * stride):
                    continue
                for k in range(cnt):
                    r = recs + k * stride
                    if idx == 9:
                        di = img.u16(r + 0xA) & 0x1F
                        per_id[door_ids[di]][lvl] += 1
                        continue
                    raw = img.u16(r)
                    if raw < len(raw2act):
                        per_id[raw2act[raw]][lvl] += 1
        if not ok:
            undec.append((lvl, ovid, lo, "no decodable table"))

    if a.id is not None:
        c = per_id.get(a.id)
        if not c:
            print("actor id %d is placed on NO level (star filter %d)"
                  % (a.id, a.star))
        else:
            print("actor id %d, star filter %d:" % (a.id, a.star))
            for lvl in sorted(c):
                print("   level %-3d x%d  (object overlay ov%03d)"
                      % (lvl, c[lvl], a32(LEVEL_OV + lvl * 4)))
        print("undecodable level rows: %d %s"
              % (len(undec), [u[0] for u in undec]))
        return

    for aid in sorted(per_id):
        c = per_id[aid]
        print("%-4d total %-4d levels %s"
              % (aid, sum(c.values()), sorted(c)))
    print("undecodable level rows: %d %s" % (len(undec), [u[0] for u in undec]))


if __name__ == "__main__":
    main()
