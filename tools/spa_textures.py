#!/usr/bin/env python3
"""Export the particle resource's textures (see tools/spa_dump.py) as PNGs.

Each texture record is a 0x20-byte header (" TPS" magic, params, texelSize,
paletteOffset, paletteSize, ..., size) then texels, then a palette.  Formats
are the NDS ones: 1 A3I5, 2 4-color, 3 16-color, 4 256-color, 5 4x4-texel
compressed, 6 A5I3, 7 direct.  Palette index 0 is drawn transparent for the
paletted formats (4-color/16-color/256-color); that is the usual sprite
convention but is not proven here, and A3I5/A5I3 carry their own alpha.

Usage:
  python tools/spa_textures.py OUTDIR            # one PNG per texture + sheet.png
  python tools/spa_textures.py OUTDIR --scale 8
Requires Pillow.  Output is for looking at, not for committing.
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import spa_dump  # noqa: E402
from PIL import Image, ImageDraw  # noqa: E402


def rgb555(v):
    return ((v & 31) * 255 // 31, (v >> 5 & 31) * 255 // 31, (v >> 10 & 31) * 255 // 31)


def decode(rec, fmt, w, h, transparent0=True):
    texel_size, pal_off, pal_size = struct.unpack_from("<III", rec, 8)
    texels = rec[0x20:0x20 + texel_size]
    pal = [rgb555(v) for v in struct.unpack_from("<%dH" % (pal_size // 2), rec, pal_off)] if pal_size else []
    img = Image.new("RGBA", (w, h))
    px = img.load()

    def put(x, y, i, bits):
        c = pal[i] if i < len(pal) else (255, 0, 255)
        px[x, y] = c + ((0,) if transparent0 and i == 0 else (255,))

    if fmt in (2, 3, 4):
        bits = {2: 2, 3: 4, 4: 8}[fmt]
        per = 8 // bits
        for n in range(w * h):
            byte = texels[n // per]
            i = (byte >> (bits * (n % per))) & ((1 << bits) - 1)
            put(n % w, n // w, i, bits)
    elif fmt in (1, 6):
        ib, ab = (5, 3) if fmt == 1 else (3, 5)
        for n in range(w * h):
            v = texels[n]
            i = v & ((1 << ib) - 1)
            a = v >> ib
            c = pal[i] if i < len(pal) else (255, 0, 255)
            px[n % w, n // w] = c + ((a * 255 // ((1 << ab) - 1)),)
    elif fmt == 7:
        for n in range(w * h):
            v = struct.unpack_from("<H", texels, n * 2)[0]
            px[n % w, n // w] = rgb555(v) + ((255 if v & 0x8000 else 0),)
    elif fmt == 5:
        # 4x4-texel compressed: texelSize bytes of 4x4 blocks (32 bits each),
        # then texelSize/2 bytes of 16-bit palette-index words.
        idx = rec[0x20 + texel_size:0x20 + texel_size + texel_size // 2]
        bw = w // 4
        for b in range(w * h // 16):
            word = struct.unpack_from("<I", texels, b * 4)[0]
            info = struct.unpack_from("<H", idx, b * 2)[0]
            base = (info & 0x3FFF) * 2
            mode = info >> 14

            def pc(k):
                j = base + k
                return pal[j] if j < len(pal) else (255, 0, 255)
            c = [pc(0), pc(1)]
            if mode == 0:
                c += [pc(2), (0, 0, 0)]
                alpha = [255, 255, 255, 0]
            elif mode == 1:
                c += [tuple((a + b2) // 2 for a, b2 in zip(c[0], c[1])), (0, 0, 0)]
                alpha = [255, 255, 255, 0]
            elif mode == 2:
                c += [pc(2), pc(3)]
                alpha = [255] * 4
            else:
                c += [tuple((a * 5 + b2 * 3) // 8 for a, b2 in zip(c[0], c[1])),
                      tuple((a * 3 + b2 * 5) // 8 for a, b2 in zip(c[0], c[1]))]
                alpha = [255] * 4
            for t in range(16):
                v = (word >> (2 * t)) & 3
                px[(b % bw) * 4 + t % 4, (b // bw) * 4 + t // 4] = tuple(c[v]) + (alpha[v],)
    else:
        raise SystemExit("unknown texture format %d" % fmt)
    return img


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("outdir")
    ap.add_argument("--arm9", default=os.path.join(here, "..", "extracted", "arm9_dec.bin"))
    ap.add_argument("--scale", type=int, default=4)
    a = ap.parse_args()
    with open(a.arm9, "rb") as f:
        data = f.read()
    t = spa_dump.parse(data)
    b = data[t["arm9_file_offset"]:]
    users = {}
    for d in t["definitions"]:
        users.setdefault(d["texture"], []).append(d["id"])
    os.makedirs(a.outdir, exist_ok=True)
    tiles = []
    for x in t["textures"]:
        rec = b[x["offset"]:x["offset"] + x["size"]]
        img = decode(rec, x["format"], x["width"], x["height"])
        img.save(os.path.join(a.outdir, "tex_%02d.png" % x["index"]))
        tiles.append((x, img))
    s, cell, cols = a.scale, 8 * 8 * a.scale // 2 + 24, 8
    cw = max(i.width for _, i in tiles) * s + 12
    ch = max(i.height for _, i in tiles) * s + 28
    rows = (len(tiles) + cols - 1) // cols
    sheet = Image.new("RGBA", (cols * cw, rows * ch), (60, 60, 70, 255))
    dr = ImageDraw.Draw(sheet)
    for n, (x, img) in enumerate(tiles):
        ox, oy = (n % cols) * cw + 6, (n // cols) * ch + 18
        dr.text((ox, oy - 14), "%d %s %dx%d  x%d" % (x["index"], x["format_name"], x["width"],
                x["height"], len(users.get(x["index"], []))), fill=(255, 255, 255, 255))
        big = img.resize((img.width * s, img.height * s), Image.NEAREST)
        sheet.paste(big, (ox, oy), big)
    sheet.save(os.path.join(a.outdir, "sheet.png"))
    print("wrote %d textures + sheet.png to %s" % (len(tiles), a.outdir))


if __name__ == "__main__":
    main()
