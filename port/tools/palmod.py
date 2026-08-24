#!/usr/bin/env python3
"""palmod.py -- compile a human-written palette combo into the game's .pal blob.

The bridge has two ends. The Studio (or a hand-written JSON) speaks colors;
the game's file layer (port/hal/fs_mods.cpp) speaks one dumb binary it can
parse in a dozen lines at boot. This tool is the crossing.

Targets are keyed by NAME on both axes -- the file's path and the palette's
own name inside it -- never by numeric id: ids are generated from each
player's own ROM dump, so a shipped id would be a guess where a shipped
name is a fact. The game resolves both names once, when the combo loads.

JSON in (one combo):

    {
      "version": 2,
      "name": "combo 1",
      "patches": [
        { "file": "data/player/yoshi_model.bmd",
          "pal_name": "yoshi_all_16p_pl",
          "colors": ["#f8f8f8", "#a03828", "..."] }   # full replacement set
      ]
    }

.pal out (little-endian), matching fs_mods.cpp exactly:

    magic  "PALM"        4 bytes
    u16    version = 2
    u16    record count
    then per record:
    u8     pathLen        u8 nameLen        u16 colorCount
    char   path[pathLen]  char name[nameLen]     (no NULs)
    u16    colors[count]  RGB555 as the DS stores palettes

Colors accept "#rgb", "#rrggbb" (bit15 set), a bare int, or "0x1234" hex
string (raw RGB555 used as-is, bit15 included -- the exact-bytes escape
hatch, and the form the survey's inventory.json dumps).

Modes:
    compile  spec.json out.pal            the normal path
    test     inventory.json spec.json [deg]   hue-rotate every palette of the
                                          twelve adventure character models
                                          into a spec (proof runs)
"""
import colorsys
import json
import struct
import sys


def rgb555_from_hex(s):
    s = s.lstrip("#")
    if len(s) == 3:
        r, g, b = (int(c, 16) * 17 for c in s)
    elif len(s) == 6:
        r, g, b = int(s[0:2], 16), int(s[2:4], 16), int(s[4:6], 16)
    else:
        raise ValueError(f"bad color {s!r}")
    return 0x8000 | (b >> 3) << 10 | (g >> 3) << 5 | (r >> 3)


def color_word(c):
    if isinstance(c, int):
        return c & 0xFFFF
    if isinstance(c, str) and c.lower().startswith("0x"):
        return int(c, 16) & 0xFFFF
    return rgb555_from_hex(c)


def compile_spec(spec_path, out_path):
    spec = json.load(open(spec_path, encoding="utf-8"))
    if spec.get("version") != 2:
        sys.exit(f"spec version {spec.get('version')!r} is not 2")
    records = []
    for p in spec["patches"]:
        path = p["file"].encode("ascii")
        name = p["pal_name"].encode("ascii")
        colors = [color_word(c) for c in p["colors"]]
        if not (0 < len(path) < 128 and 0 < len(name) < 64 and
                0 < len(colors) <= 4096):
            sys.exit(f"record out of bounds: {p['file']} {p['pal_name']}")
        records.append(struct.pack("<BBH", len(path), len(name), len(colors))
                       + path + name
                       + struct.pack("<%dH" % len(colors), *colors))
    blob = b"PALM" + struct.pack("<HH", 2, len(records)) + b"".join(records)
    with open(out_path, "wb") as f:
        f.write(blob)
    print(f"{out_path}: {len(records)} patch records, {len(blob)} bytes")


def hue_rotate(word, degrees):
    """Rotate an RGB555 word's hue, keep bit15 as stored."""
    a = word & 0x8000
    r = (word & 0x1F) / 31.0
    g = (word >> 5 & 0x1F) / 31.0
    b = (word >> 10 & 0x1F) / 31.0
    h, l, s = colorsys.rgb_to_hls(r, g, b)
    h = (h + degrees / 360.0) % 1.0
    r, g, b = colorsys.hls_to_rgb(h, l, s)
    return a | int(b * 31 + .5) << 10 | int(g * 31 + .5) << 5 | int(r * 31 + .5)


# the twelve files Player::InitResources loads unconditionally (survey)
TEST_ROLES = {"body", "head_cap", "head_nocap", "head cap", "head nocap"}


def make_test(inventory_path, out_path, degrees=150):
    inv = json.load(open(inventory_path, encoding="utf-8"))
    patches = []
    for f in inv["files"]:
        if f.get("role", "").replace(" ", "_") not in {"body", "head_cap",
                                                       "head_nocap"}:
            continue
        for pal in f.get("palettes", []):
            words = [int(c, 16) for c in pal["colors_bgr555"]]
            patches.append({
                "file": f["path"],
                "pal_name": pal["name"],
                "colors": [f"0x{hue_rotate(w, degrees):04x}" for w in words],
            })
    spec = {"version": 2, "name": f"test hue+{degrees}", "patches": patches}
    json.dump(spec, open(out_path, "w", encoding="utf-8"), indent=1)
    print(f"{out_path}: {len(patches)} patches over "
          f"{len({p['file'] for p in patches})} files")


def main():
    if len(sys.argv) == 4 and sys.argv[1] == "compile":
        compile_spec(sys.argv[2], sys.argv[3])
    elif len(sys.argv) in (4, 5) and sys.argv[1] == "test":
        deg = int(sys.argv[4]) if len(sys.argv) == 5 else 150
        make_test(sys.argv[2], sys.argv[3], deg)
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
