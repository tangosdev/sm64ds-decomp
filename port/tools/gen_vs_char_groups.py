#!/usr/bin/env python3
"""gen_vs_char_groups.py -- bake the bros' colour categories into
port/hal/vs_char_groups.h, read straight out of the player's own extracted ROM.

WHAT THIS IS. Mario, Luigi and Wario can be recoloured by CATEGORY (the online
profile popup's HAT, OVERALLS and SHOES rows, and the dev knob
SM64DS_VS_CHAR_COLORS). The arithmetic is the Yoshi path's family transform
(port/hal/vs_palette_gen.h through vs_char_palette_gen.h); what it needs per
category is WHICH palette entries of WHICH bro palette belong to it. This
script finds them in the cartridge files and writes the table.

WHY NOT THE STUDIO'S groups.json ANY MORE (run after093, lane BROCOLOR1). The
first table came from the palette editor's hand overrides and covered about 94
texels: Mario had no hat, Luigi and Mario no shoes, nobody any buttons. The
bros' textures are format 5 (4x4 compressed): every 4x4 block reads two to
four palette entries of its own, so a body palette holds 250 to 500 entries and
a category is a scatter of a hundred of them. Hand-picking cannot cover that;
classifying every entry can.

HOW AN ENTRY IS CLASSIFIED. Every non-eye palette of the nine bro files
(data/player/<bro>_model.bmd, <bro>_head_cap.bmd, <bro>_head_nocap.bmd, in
ARCHIVE/arc0.narc) is decoded against the texture its material draws it with,
crediting each texel to the entries it reads (a blended texel half to each
end). An entry joins a category when
  * its colour is the cartridge's colour for that category on that bro
    (hue / saturation / value windows below: Mario red / blue / brown, Luigi
    green / navy / brown, Wario yellow / purple / green), and
  * where the file mixes that colour with other parts of the body, at least
    half of its texels lie in the category's part of the texture (RECTS:
    Mario's cap is the left column of his head texture, his hair and moustache
    are dark red-brown too; his shoe tile is the bottom row's second tile; the
    glove tile's red dot is not his shirt).
Heads carry no overalls and no shoes. Luigi's second body / head pair
(luigi_body_tr_2 + luigi_body_2_pl, luigi_head_2 + luigi_head_2_pl, the pair
L_tx_bodytrans.btp / L_tx_headtrans.btp switch to) does not decode as a
picture against the companion data the file carries, so it is classified by
colour alone, which is all a palette entry has anyway.

THE MAIN TONE. For each category in each palette the entry the most texels
read is written as `main`: the pick lands exactly there and the rest of the
category follows as a ramp (vs_char_palette_gen.h). kBroCart is the cartridge
colour of each bro's category, the main tone in the BODY file: the popup's
classic cells, and a pick equal to it leaves the palette untouched.

Run from the worktree root:
    python port/tools/gen_vs_char_groups.py [--files extracted/dsd/files]
        [--out port/hal/vs_char_groups.h] [--json <dump>]
"""
import argparse
import colorsys
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lovesme_compose as lc  # noqa: E402

CHARS = ["mario", "luigi", "wario"]          # engine order: 0 Mario, 1 Luigi, 2 Wario
CAT_HAT, CAT_OVERALLS, CAT_GLOVES, CAT_BUTTONS, CAT_SHOES = 0, 1, 2, 3, 4
CAT_NAMES = ["hat_undershirt", "overalls", "gloves", "buttons", "shoes"]

# arc0.narc member index, file path served to the game, is it the body file
FILES = {
    "mario": [(131, "data/player/mario_model.bmd", True), (129, "data/player/mario_head_cap.bmd", False),
              (130, "data/player/mario_head_nocap.bmd", False)],
    "luigi": [(128, "data/player/luigi_model.bmd", True), (126, "data/player/luigi_head_cap.bmd", False),
              (127, "data/player/luigi_head_nocap.bmd", False)],
    "wario": [(186, "data/player/wario_model.bmd", True), (184, "data/player/wario_head_cap.bmd", False),
              (185, "data/player/wario_head_nocap.bmd", False)],
}

# texture x0, y0, x1, y1 (texels) a category's entries must mostly sit in, per palette; absent = anywhere
RECTS = {
    ("mario_body_pl", CAT_HAT): [(0, 0, 64, 48)],
    ("mario_body_pl", CAT_OVERALLS): [(0, 0, 64, 48)],
    ("mario_body_pl", CAT_SHOES): [(10, 48, 26, 64)],
    ("mario_head_pl", CAT_HAT): [(0, 0, 16, 48)],
    ("mario_head_n_pl", CAT_HAT): [],              # no cap on this head
}


def hsv(v):
    r, g, b = (v & 31) / 31.0, ((v >> 5) & 31) / 31.0, ((v >> 10) & 31) / 31.0
    h, s, val = colorsys.rgb_to_hsv(r, g, b)
    return h * 360.0, s, val


def hue_in(h, a, b):
    return (a <= h <= b) if a <= b else (h >= a or h <= b)


def colour_cat(chr_name, body, v):
    h, s, val = hsv(v)
    if s < 0.35 or val < 0.10:
        return -1
    if chr_name == "mario":
        if hue_in(h, 300, 12) and s > 0.55:
            return CAT_HAT
        if body and hue_in(h, 200, 290):
            return CAT_OVERALLS
        if body and hue_in(h, 12, 40) and s > 0.55 and val < 0.9:
            return CAT_SHOES
    elif chr_name == "luigi":
        if hue_in(h, 75, 165) and s > 0.45:
            return CAT_HAT
        if body and hue_in(h, 195, 280):
            return CAT_OVERALLS
        if body and hue_in(h, 0, 45) and val < 0.62:
            return CAT_SHOES
    else:
        if hue_in(h, 40, 70) and s > 0.55 and val > 0.35:
            return CAT_HAT
        if body and hue_in(h, 265, 330):
            return CAT_OVERALLS
        if body and hue_in(h, 75, 165):
            return CAT_SHOES
    return -1


def decode_refs(tex, ncol):
    """per texel: list of (palette entry, weight), and the texture width / height"""
    (flags,) = struct.unpack_from("<I", tex["rec"], 0x10)
    fmt = (flags >> 26) & 7
    w, h = 8 << ((flags >> 20) & 7), 8 << ((flags >> 23) & 7)
    refs = [()] * (w * h)
    data = tex["data"]
    if fmt == 5:
        sz = tex["size_field"]
        main, comp = data[:sz], data[sz:]
        for bi in range(len(main) // 4):
            (block,) = struct.unpack_from("<I", main, bi * 4)
            (info,) = struct.unpack_from("<H", comp, bi * 2)
            base, mode = (info & 0x3fff) * 2, info >> 14
            bx, by = (bi % (w // 4)) * 4, (bi // (w // 4)) * 4
            for k in range(16):
                c = (block >> (k * 2)) & 3
                p = (by + k // 4) * w + bx + k % 4
                if c == 3 and mode in (0, 1):
                    continue
                if (mode == 1 and c == 2) or (mode == 3 and c >= 2):
                    refs[p] = ((base, 0.5), (base + 1, 0.5))
                else:
                    refs[p] = ((base + c, 1.0),)
    else:
        raise SystemExit("unexpected texture format %d in %s" % (fmt, tex["name"]))
    return w, h, [tuple((k, wt) for k, wt in r if k < ncol) for r in refs]


def texture_for(m, pal_name):
    """the texture a palette is drawn with: the material pairing, else the name pairing of the BTP swap pairs"""
    pi = [i for i, p in enumerate(m["pals"]) if p["name"] == pal_name][0]
    for mat in m["mats"]:
        ti, pj = struct.unpack_from("<ii", mat["rec"], 4)
        if pj == pi and ti >= 0:
            return m["texs"][ti], True
    pairs = {"luigi_head_1_pl": "luigi_head_1", "luigi_body_2_pl": "luigi_body_tr_2"}
    for t in m["texs"]:
        if t["name"] == pairs.get(pal_name):
            return t, pal_name == "luigi_head_1_pl"
    return None, False


def classify(chr_name, body, m, pal):
    words = [struct.unpack_from("<H", pal["data"], i * 2)[0] for i in range(len(pal["data"]) // 2)]
    cats = [colour_cat(chr_name, body, v) for v in words]
    tex, picture = texture_for(m, pal["name"])
    cnt = [0.0] * len(words)
    if tex is not None:
        w, h, refs = decode_refs(tex, len(words))
        if pal["name"] in ("luigi_head_2_pl",):      # material pairing decodes as noise: colour only
            picture = False
        inside = [[0.0] * len(words) for _ in range(5)]
        for i, r in enumerate(refs):
            x, y = i % w, i // w
            for k, wt in r:
                cnt[k] += wt
                for c in range(5):
                    for (x0, y0, x1, y1) in RECTS.get((pal["name"], c), []):
                        if x0 <= x < x1 and y0 <= y < y1:
                            inside[c][k] += wt
        if picture:
            for k, c in enumerate(cats):
                if c >= 0 and (pal["name"], c) in RECTS and inside[c][k] * 2 < cnt[k]:
                    cats[k] = -1
    return words, cats, cnt


def rgb8(v):
    return (((v & 31) * 255 // 31) << 16) | ((((v >> 5) & 31) * 255 // 31) << 8) | (((v >> 10) & 31) * 255 // 31)


def collect(files_root):
    arc = os.path.join(files_root, "ARCHIVE", "arc0.narc")
    records, dump, cart = [], {}, [[0] * 5 for _ in CHARS]
    for ci, chr_name in enumerate(CHARS):
        for idx, path, body in FILES[chr_name]:
            m = lc.parse_bmd(lc.narc_member(arc, idx))
            for pal in m["pals"]:
                if "eye" in pal["name"]:
                    continue
                if not pal["name"].startswith(chr_name):
                    raise SystemExit("arc0 member %d is not %s (palette %s)" % (idx, path, pal["name"]))
                words, cats, cnt = classify(chr_name, body, m, pal)
                dump[pal["name"]] = {"file": path, "chr": ci, "cats": cats, "cnt": cnt, "words": words}
                for c in range(5):
                    members = [k for k, x in enumerate(cats) if x == c]
                    if not members:
                        continue
                    main = max(members, key=lambda k: (cnt[k], -k))
                    records.append({"chr": ci, "cat": c, "file": path, "pal": pal["name"], "idx": members,
                                    "main": main, "drawn": sum(cnt[k] for k in members)})
                    if body and pal["name"] in ("mario_body_pl", "luigi_body_1_pl", "wario_body_pl"):
                        cart[ci][c] = rgb8(words[main])
    records.sort(key=lambda r: (r["chr"], r["cat"], r["file"], r["pal"]))
    return records, dump, cart


def emit(records, cart):
    out = []
    w = out.append
    w("/* vs_char_groups.h -- GENERATED by port/tools/gen_vs_char_groups.py from the")
    w(" * extracted ROM (ARCHIVE/arc0.narc, the nine bro BMDs). DO NOT EDIT BY HAND.")
    w(" *")
    w(" * The bros' category -> palette-entry membership: hat + undershirt (one")
    w(" * colour, cap in the head file and shirt in the body file), overalls, shoes.")
    w(" * Gloves and buttons keep their slots for SM64DS_VS_CHAR_COLORS's grammar and")
    w(" * are empty. Each record is one category's entries in one named palette;")
    w(" * `main` is the entry the most texels read (where a pick lands). kBroCart is")
    w(" * each bro's cartridge colour per category (the body file's main tone).")
    w(" *")
    w(" * COVERAGE (entries / texels drawn with them):")
    for ci, chr_name in enumerate(CHARS):
        for c in (CAT_HAT, CAT_OVERALLS, CAT_SHOES):
            rs = [r for r in records if r["chr"] == ci and r["cat"] == c]
            w(" *   %-6s %-15s %s" % (chr_name, CAT_NAMES[c], ", ".join(
                "%s %d/%d" % (r["pal"], len(r["idx"]), int(r["drawn"] + 0.5)) for r in rs) or "-"))
    w(" */")
    w("#ifndef PORT_HAL_VS_CHAR_GROUPS_H")
    w("#define PORT_HAL_VS_CHAR_GROUPS_H")
    w("")
    w("namespace vscg {")
    w("")
    w("enum { CAT_HAT_UNDERSHIRT = 0, CAT_OVERALLS = 1, CAT_GLOVES = 2,")
    w("       CAT_BUTTONS = 3, CAT_SHOES = 4, CAT_COUNT = 5 };")
    w("")
    w("struct Group {")
    w("    int         chr;    /* 0 Mario, 1 Luigi, 2 Wario */")
    w("    int         cat;    /* 0..4, the category above */")
    w("    const char *file;   /* served BMD path, e.g. data/player/mario_model.bmd */")
    w("    const char *pal;    /* palette name inside that BMD, e.g. mario_body_pl */")
    w("    int         count;  /* number of entries */")
    w("    const int  *idx;    /* the palette entries this category owns, ascending */")
    w("    int         main;   /* the most-drawn entry of them */")
    w("};")
    w("")
    for i, r in enumerate(records):
        vals = [str(x) for x in r["idx"]]
        rows = [", ".join(vals[j:j + 16]) for j in range(0, len(vals), 16)]
        w("static const int kIdx_%d[%d] = {" % (i, len(vals)))
        for j, row in enumerate(rows):
            w("    %s%s" % (row, "," if j + 1 < len(rows) else ""))
        w("};")
    w("")
    w("static const Group kVscGroups[] = {")
    for i, r in enumerate(records):
        w('    { %d, %d, "%s", "%s", %d, kIdx_%d, %d },' % (r["chr"], r["cat"], r["file"], r["pal"], len(r["idx"]), i,
                                                            r["main"]))
    w("};")
    w("static const int kVscGroupCount = %d;" % len(records))
    w("")
    w("/* 0xRRGGBB, [chr][cat]; 0 = no such category on that bro */")
    w("static const unsigned kBroCart[3][CAT_COUNT] = {")
    for ci in range(3):
        w("    { %s }," % ", ".join("0x%06Xu" % v for v in cart[ci]))
    w("};")
    w("")
    w("} /* namespace vscg */")
    w("")
    w("#endif /* PORT_HAL_VS_CHAR_GROUPS_H */")
    return "\n".join(out) + "\n"


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    ap = argparse.ArgumentParser()
    ap.add_argument("--files", default=os.path.normpath(os.path.join(here, "..", "..", "extracted", "dsd", "files")))
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..", "hal", "vs_char_groups.h")))
    ap.add_argument("--json", default=None)
    a = ap.parse_args()
    records, dump, cart = collect(a.files)
    for r in records:
        print("  %-6s %-15s %-16s %3d entries  main %3d  %6.0f texels" % (
            CHARS[r["chr"]], CAT_NAMES[r["cat"]], r["pal"], len(r["idx"]), r["main"], r["drawn"]), file=sys.stderr)
    with open(a.out, "w", encoding="ascii", newline="\n") as f:
        f.write(emit(records, cart))
    if a.json:
        with open(a.json, "w") as f:
            json.dump(dump, f)
    print("wrote %s" % a.out, file=sys.stderr)


if __name__ == "__main__":
    main()
