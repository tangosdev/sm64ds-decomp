#!/usr/bin/env python3
"""vs_rows16_bmd_check.py -- prove the sixteen-row Yoshi palette surgery.

WHAT THIS CHECKS AND WHAT IT DOES NOT. hal/fs_mods.cpp's yoshi_rows16_grow
takes the served data/player/yoshi_model.bmd, appends a 512-byte palette at the
end of the file and repoints the yoshi_all_16p_pl record at it, so that VS can
give each of sixteen seats its own sixteen-colour row. (512 appended, not 384:
the cartridge's own 128 bytes cannot be the first quarter of the new palette,
because the bytes after them belong to other records. 384 is the extra palette
VRAM the grown record asks for, which is a different number.) Two claims
have to hold for that to be safe, and they are file-shape claims rather than
colour claims:

  1. NOTHING ELSE IN THE FILE MOVES. Every other offset -- bones, display
     lists, textures, materials, the bone-slot table -- still means what it
     meant, and every record still lies inside the file.
  2. THE GROWN FILE STILL PARSES, by the same reader the port and the composer
     use, and the palette comes back at the new offset with the new size.

This script reproduces the surgery in Python against the REAL cartridge file
and asserts both, plus that rows 0..3 come through byte-identical to the ROM's
own four rows (kVsRomRows in hal/vs_palette_gen.h, pasted below from the same
source the header quotes).

The COLOUR arithmetic is not checked here -- port/tools/vs_palette_test.cpp
owns that, against the shipped header -- and neither is the runtime upload,
which only a real run can show. This is the middle link: the bytes on the way
in.

    python port/tools/vs_rows16_bmd_check.py [--files <extracted files dir>]
"""

import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lovesme_compose as lc          # narc_member, parse_bmd, palette_named, cstr

YOSHI_MEMBER = 196
PAL_NAME = "yoshi_all_16p_pl"

# hal/vs_palette_gen.h kVsRomRows, verbatim.
ROM_ROWS = [
    [0x0d83, 0x1224, 0x1a86, 0x2eab, 0x0022, 0x075d, 0x084e, 0x0d36,
     0x0d9c, 0x107c, 0x1093, 0x1a99, 0x56d7, 0x675a, 0x77bd, 0x7fff],
    [0x1094, 0x083b, 0x1cdf, 0x357c, 0x0022, 0x075d, 0x084e, 0x6500,
     0x7d20, 0x107c, 0x1093, 0x167a, 0x56d7, 0x675a, 0x77bd, 0x7fff],
    [0x34c5, 0x4d07, 0x5949, 0x72ad, 0x0022, 0x075d, 0x084e, 0x4c33,
     0x5856, 0x107c, 0x1093, 0x0dbd, 0x56d7, 0x675a, 0x77bd, 0x7fff],
    [0x167c, 0x22de, 0x033f, 0x579e, 0x0022, 0x075d, 0x084e, 0x02a6,
     0x06e6, 0x107c, 0x1093, 0x167d, 0x56d7, 0x675a, 0x77bd, 0x7fff],
]

FAIL = []


def check(what, cond):
    print("  %s %s" % ("ok  " if cond else "FAIL", what))
    if not cond:
        FAIL.append(what)


def pal_record(d, name):
    """(record offset, data offset, size) for a palette, by its own name."""
    h = struct.unpack_from("<15I", d, 0)
    n_pal, pal_off = h[7], h[8]
    for i in range(n_pal):
        rec = pal_off + i * 0x10
        nm, dp, sz = struct.unpack_from("<III", d, rec)
        if lc.cstr(d, nm) == name:
            return rec, dp, sz
    raise KeyError(name)


def grow(d):
    """The Python twin of hal/fs_mods.cpp yoshi_rows16_grow's FILE surgery.

    The twelve generated rows are not computed here -- that is the shipped
    header's job and vs_palette_test's -- so rows 4..15 are written as a
    recognisable marker pattern. What is under test is where the bytes land
    and what the record says about them."""
    rec, dp, sz = pal_record(d, PAL_NAME)
    assert sz == 128, sz
    rows = [list(struct.unpack_from("<16H", d, dp + r * 32)) for r in range(4)]
    out = list(rows)
    for s in range(4, 16):
        out.append([(0xA000 | (s << 4) | i) & 0x7fff for i in range(16)])

    newoff = (len(d) + 3) & ~3
    n = bytearray(d) + bytearray(newoff - len(d)) + bytearray(512)
    for r in range(16):
        struct.pack_into("<16H", n, newoff + r * 32, *out[r])
    struct.pack_into("<II", n, rec + 4, newoff, 512)
    return bytes(n), newoff, rec


def record_extents(d):
    """Every (name, offset, length) span the BMD's own records point at, so a
    grown file can be compared span for span against the original."""
    h = struct.unpack_from("<15I", d, 0)
    nb, bo, ndl, dlo, ntex, texo, npal, palo, nmat, mato = h[1:11]
    spans = [("bones", bo, nb * 0x40), ("dltab", dlo, ndl * 8),
             ("textab", texo, ntex * 0x14), ("paltab", palo, npal * 0x10),
             ("mattab", mato, nmat * 0x30)]
    for i in range(nb):
        rec = bo + i * 0x40
        cnt = struct.unpack_from("<I", d, rec + 0x30)[0]
        mi, di = struct.unpack_from("<II", d, rec + 0x34)
        spans.append(("bone%d.mats" % i, mi, cnt))
        spans.append(("bone%d.dls" % i, di, cnt))
    for i in range(ndl):
        sub = struct.unpack_from("<I", d, dlo + i * 8 + 4)[0]
        mc, mp, ss, sp = struct.unpack_from("<4I", d, sub)
        spans.append(("dl%d.map" % i, mp, mc))
        spans.append(("dl%d.stream" % i, sp, ss))
    for i in range(ntex):
        rec = texo + i * 0x14
        dp, sz = struct.unpack_from("<II", d, rec + 4)
        flags = struct.unpack_from("<I", d, rec + 0x10)[0]
        blob = sz + sz // 2 if ((flags >> 26) & 7) == 5 else sz
        spans.append(("tex%d" % i, dp, blob))
    for i in range(npal):
        rec = palo + i * 0x10
        dp, sz = struct.unpack_from("<II", d, rec + 4)
        spans.append(("pal%d" % i, dp, sz))
    return spans


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--files", default=None,
                    help="the dsd-extracted files dir (…/extracted/dsd/files)")
    a = ap.parse_args()

    root = a.files
    if not root:
        here = os.path.dirname(os.path.dirname(os.path.dirname(
            os.path.abspath(__file__))))
        root = os.path.join(here, "extracted", "dsd", "files")
    arc = os.path.join(root, "ARCHIVE", "arc0.narc")
    if not os.path.isfile(arc):
        print("no arc0.narc under %s; pass --files" % root)
        return 2
    d = lc.narc_member(arc, YOSHI_MEMBER)

    print("source: %s member %d, %d bytes" % (arc, YOSHI_MEMBER, len(d)))

    print("the cartridge's own shape")
    rec0, dp0, sz0 = pal_record(d, PAL_NAME)
    check("%s is 128 bytes" % PAL_NAME, sz0 == 128)
    rows0 = [list(struct.unpack_from("<16H", d, dp0 + r * 32)) for r in range(4)]
    check("its four rows are kVsRomRows", rows0 == ROM_ROWS)

    n, newoff, rec = grow(d)

    print("the grown file")
    # A WHOLE 512 IS APPENDED, not 384. The cartridge's 128 bytes cannot be
    # reused as the first quarter of the new palette -- the 384 bytes that
    # follow them in the file belong to other records -- so the four ROM rows
    # are copied up with the twelve new ones and the old 128 are left where
    # they are as dead space. The 384 figure is the extra PALETTE VRAM the
    # grown record asks the allocator for, which is a different number.
    check("grew by exactly 512 bytes plus alignment padding",
          len(n) == ((len(d) + 3) & ~3) + 512)
    check("the appended block starts at or after the old end", newoff >= len(d))
    # The only bytes below the original length that may differ are the eight
    # in the palette record itself, and only the ones whose VALUE moved.
    diff = [i for i in range(len(d)) if n[i] != d[i]]
    check("nothing below the old length changed except the palette record's "
          "own offset and size words: %s"
          % ([hex(i) for i in diff] or "-"),
          diff and all(rec + 4 <= i < rec + 12 for i in diff))

    rec1, dp1, sz1 = pal_record(n, PAL_NAME)
    check("the record still has the same name and place", rec1 == rec0)
    check("it now says 512 bytes", sz1 == 512)
    check("at the appended offset", dp1 == newoff)
    check("and 512 bytes are really there", dp1 + sz1 <= len(n))
    check("the palette is four-byte aligned", dp1 % 4 == 0)

    rows1 = [list(struct.unpack_from("<16H", n, dp1 + r * 32)) for r in range(16)]
    check("rows 0..3 are the cartridge's, byte for byte", rows1[:4] == ROM_ROWS)
    check("rows 4..15 are all present and all distinct",
          len(set(tuple(r) for r in rows1[4:])) == 12)

    print("every other record still points where it pointed")
    a_spans = dict((k, (o, l)) for k, o, l in record_extents(d))
    b_spans = dict((k, (o, l)) for k, o, l in record_extents(n))
    check("the same set of records", set(a_spans) == set(b_spans))
    moved = [k for k in a_spans
             if a_spans[k] != b_spans[k] and k != "pal%d" % 0]
    moved = [k for k in a_spans if a_spans[k] != b_spans[k]
             and not (k.startswith("pal") and b_spans[k][1] == 512)]
    check("nothing but the Yoshi palette moved or resized: %s" % (moved or "-"),
          not moved)
    oob = [k for k, (o, l) in b_spans.items() if o + l > len(n)]
    check("every record lies inside the grown file: %s" % (oob or "-"), not oob)

    print("it still parses")
    try:
        m = lc.parse_bmd(n)
        ok = True
    except Exception as e:              # noqa: BLE001 -- the check IS the catch
        print("       parse_bmd raised: %r" % (e,))
        ok = False
    check("parse_bmd accepts the grown file", ok)
    if ok:
        p = [e for e in m["pals"] if e["name"] == PAL_NAME]
        check("and reads the palette back as 512 bytes",
              len(p) == 1 and p[0]["size_field"] == 512
              and len(p[0]["data"]) == 512)
        m0 = lc.parse_bmd(d)
        check("with the same texture, material and bone counts as before",
              len(m["texs"]) == len(m0["texs"])
              and len(m["mats"]) == len(m0["mats"])
              and len(m["bones"]) == len(m0["bones"])
              and m["slots"] == m0["slots"])

    print("\n%s" % ("FAILED: " + "; ".join(FAIL) if FAIL else "all green"))
    return 1 if FAIL else 0


if __name__ == "__main__":
    sys.exit(main())
