#!/usr/bin/env python3
"""Dump the particle effect table (SPA resource) embedded in arm9.

`Particle::System::NewSimple(id, x, y, z)` -> `Manager::AddSystem(id, pos)` ->
`mDefinitions[id]`.  That table is one SPA resource ("SPA " magic, stored as
b' APS') parsed at startup by func_0204a17c.  This tool walks it with the same
record sizes that function uses, so the layout is ROM-proven:

  header 0x20 | N definitions | M texture records

A definition is a 0x38-byte base record followed, in flag-bit order, by the
optional records func_0204a17c steps over (bits 8-11 and 16) and one behavior
record per set bit 24-29.  The walk must end exactly at the header's
residentSize word (0x18); the tool refuses to print a table that does not.

The ROM stores NO effect names, only indices.  Fields are raw except where
src/engine/particle names them or the disassembly proves them:

  base byte 0x33  texture index.  PROVEN: func_0204a5c8 (0x0204a5c8) does
                  `ldrb r1,[data,#0x33]; mla r0,r1,#0x14,mTextures` to bind it.
  child record +0x0f  the child effect's texture index, same code.
  flags bits 0-3  emitter shape (func_0204c584 jump-tables on flags & 0xf, 0..5).
  flags bits 4-5  draw/blend selector (0..2) read by func_0204a5c8.

Usage:
  python tools/spa_dump.py                      # table of every effect
  python tools/spa_dump.py --id 0xb3            # one effect, raw words
  python tools/spa_dump.py --textures           # the texture table
  python tools/spa_dump.py --json out.json      # everything, machine-readable
  python tools/spa_dump.py --arm9 path/to/arm9_dec.bin
"""
import argparse
import json
import os
import struct
import sys

ARM9_BASE = 0x02004000  # data_02075f14 is the resource, at arm9_dec offset 0x71f14
MAGIC = b" APS"

# (flag bit, record size, name): the optional records, in func_0204a17c order.
OPTIONAL = [
    (8, 0xC, "optionalA"),
    (9, 0xC, "optionalB"),
    (10, 0x8, "optionalC"),
    (11, 0xC, "optionalD"),
    (16, 0x14, "child"),
]
# Behavior records, bit 24..29, sizes from include/Particle__Behavior.h.
BEHAVIORS = [
    (24, 0x8, "acceleration"),
    (25, 0x8, "jitter"),
    (26, 0x10, "converge"),
    (27, 0x4, "turn"),
    (28, 0x8, "limitPlane"),
    (29, 0x10, "radiusConverge"),
]
TEXTURE_FORMATS = {
    1: "A3I5", 2: "4-color", 3: "16-color", 4: "256-color",
    5: "4x4-texel", 6: "A5I3", 7: "direct",
}
BASE_SIZE = 0x38
TEXTURE_FIELD = 0x33  # proven: func_0204a5c8 reads it as the texture index


def fx12(v):
    return v / 4096.0


def decode_base(b, start):
    """Fields of the 0x38-byte base record that the ROM's own code names.

    Sources: func_0204ae2c copies radius/axis/lifetime/interval/alpha into the live
    System (names from include/Particle__System.h); func_0204c584 switches on
    flags & 0xf; func_0204a5c8 reads the draw-mode bits and the texture byte.
    """
    flags = struct.unpack_from("<I", b, start)[0]
    radius = struct.unpack_from("<i", b, start + 0x08)[0]
    axis = struct.unpack_from("<3h", b, start + 0x0C)
    return {
        "shape": flags & 0xF,                 # emitter shape 0..5
        "drawMode": flags >> 4 & 3,           # 0..2, selects draw/blend routine
        "radius": fx12(radius),               # emitRadius (Fix12)
        "axis": [fx12(a) for a in axis],      # direction vector (1.3.12 fixed)
        "lifetime": struct.unpack_from("<H", b, start + 0x2A)[0],  # particleLifetime
        "emitInterval": b[start + 0x30],      # System::emitInterval
        "baseAlpha": b[start + 0x31],         # System::baseAlpha
        "texture": b[start + 0x33],
    }


def find_resource(data):
    """Return the offset of the SPA header inside data."""
    hits = []
    i = data.find(MAGIC)
    while i >= 0:
        if i + 0x20 <= len(data):
            nd, nt = struct.unpack_from("<HH", data, i + 8)
            if 0 < nd < 4096 and 0 < nt < 512 and data[i + 4:i + 8] == b"10_1":
                hits.append(i)
        i = data.find(MAGIC, i + 1)
    if len(hits) != 1:
        sys.exit("expected exactly one SPA header in the image, found %d" % len(hits))
    return hits[0]


def parse(data):
    base = find_resource(data)
    b = data[base:]
    nd, nt = struct.unpack_from("<HH", b, 8)
    resident = struct.unpack_from("<I", b, 0x18)[0]
    off = 0x20
    defs = []
    for i in range(nd):
        start = off
        flags = struct.unpack_from("<I", b, off)[0]
        off += BASE_SIZE
        records = {}
        for bit, size, name in OPTIONAL:
            if flags >> bit & 1:
                records[name] = (off - start, size)
                off += size
        behaviors = []
        for bit, size, name in BEHAVIORS:
            if flags >> bit & 1:
                behaviors.append(name)
                records[name] = (off - start, size)
                off += size
        defs.append({
            "id": i,
            "offset": start,
            "size": off - start,
            "flags": flags,
            "words": list(struct.unpack_from("<14I", b, start)),
            "texture": b[start + TEXTURE_FIELD],
            "decoded": decode_base(b, start),
            "optional": [n for _, _, n in OPTIONAL if n in records],
            "behaviors": behaviors,
            "records": {k: {"offset": o, "size": s} for k, (o, s) in records.items()},
        })
    if off != resident:
        sys.exit("definition walk ended at 0x%x, header residentSize is 0x%x: "
                 "layout assumption broken, refusing to print" % (off, resident))
    textures = []
    for i in range(nt):
        fmt_word = struct.unpack_from("<I", b, off + 4)[0]
        size = struct.unpack_from("<I", b, off + 0x1C)[0]
        fmt = fmt_word & 15
        textures.append({
            "index": i,
            "offset": off,
            "size": size,
            "format": fmt,
            "format_name": TEXTURE_FORMATS.get(fmt, "?"),
            "width": 8 << (fmt_word >> 4 & 15),
            "height": 8 << (fmt_word >> 8 & 15),
        })
        off += size
    return {
        "arm9_file_offset": base,
        "rom_address": ARM9_BASE + base,  # arm9_dec.bin is loaded at ARM9_BASE
        "numDefinitions": nd,
        "numTextures": nt,
        "residentSize": resident,
        "resourceEnd": off,
        "definitions": defs,
        "textures": textures,
    }


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--arm9", default=os.path.join(here, "..", "extracted", "arm9_dec.bin"))
    ap.add_argument("--id", type=lambda s: int(s, 0))
    ap.add_argument("--textures", action="store_true")
    ap.add_argument("--json")
    a = ap.parse_args()

    with open(a.arm9, "rb") as f:
        t = parse(f.read())

    if a.json:
        with open(a.json, "w") as f:
            json.dump(t, f, indent=1)
        print("wrote %s" % a.json)
        return
    if a.textures:
        for x in t["textures"]:
            print("tex %2d  %-9s %3dx%-3d  %5d bytes  @0x%x" % (
                x["index"], x["format_name"], x["width"], x["height"], x["size"], x["offset"]))
        return
    if a.id is not None:
        if not 0 <= a.id < t["numDefinitions"]:
            sys.exit("id out of range 0..0x%x" % (t["numDefinitions"] - 1))
        d = t["definitions"][a.id]
        print("effect 0x%x (%d)  record @0x%x size 0x%x  flags 0x%08x" % (
            a.id, a.id, d["offset"], d["size"], d["flags"]))
        print("optional:  %s" % (", ".join(d["optional"]) or "-"))
        print("behaviors: %s" % (", ".join(d["behaviors"]) or "-"))
        tx = t["textures"][d["texture"]] if d["texture"] < t["numTextures"] else None
        print("texture (byte 0x33): %d%s" % (d["texture"], "" if not tx else
              "  %s %dx%d" % (tx["format_name"], tx["width"], tx["height"])))
        x = d["decoded"]
        print("shape %d  drawMode %d  radius %.3f  axis (%.3f, %.3f, %.3f)" % (
            x["shape"], x["drawMode"], x["radius"], *x["axis"]))
        print("lifetime %d  emitInterval %d  baseAlpha %d" % (
            x["lifetime"], x["emitInterval"], x["baseAlpha"]))
        for i, w in enumerate(d["words"]):
            print("  +0x%02x  %08x" % (i * 4, w))
        return

    print("SPA @ arm9_dec 0x%x (ROM 0x%08x): %d effects, %d textures, resident 0x%x" % (
        t["arm9_file_offset"], t["rom_address"], t["numDefinitions"],
        t["numTextures"], t["residentSize"]))
    print("%-6s %-10s %-3s %-4s %-5s %-6s %-4s %-3s %-5s %s" % (
        "id", "flags", "tex", "shp", "draw", "radius", "life", "int", "alpha", "optional / behaviors"))
    for d in t["definitions"]:
        x = d["decoded"]
        extra = ",".join(d["optional"] + d["behaviors"]) or "-"
        print("0x%03x  0x%08x %3d %4d %5d %6.2f %4d %3d %5d %s" % (
            d["id"], d["flags"], d["texture"], x["shape"], x["drawMode"], x["radius"],
            x["lifetime"], x["emitInterval"], x["baseAlpha"], extra))


if __name__ == "__main__":
    main()
