#!/usr/bin/env python3
"""gen_vs_char_groups.py -- bake the bros' colour categories into
port/hal/vs_char_groups.h from Tango's own labels (port/tools/bro_groups.json)
and the player's own extracted ROM.

WHAT THIS IS. Mario, Luigi and Wario can be recoloured by CATEGORY (the online
profile popup's HAT, OVERALLS and SHOES rows, and the dev knob
SM64DS_VS_CHAR_COLORS). The arithmetic is the Yoshi path's family transform
(port/hal/vs_palette_gen.h through vs_char_palette_gen.h); what it needs per
category is WHICH palette entries of WHICH bro palette belong to it.

WHERE THE MEMBERSHIP COMES FROM (run after093, lane BROGROUPS1). Tango labelled
the bros' colours in the tangOS Studio palette editor: the editor clusters
every palette entry of the nine bro BMDs into families by rig part and colour,
and he renamed the families he meant (Hat, Undershirt, Overalls, Shoes, Gloves,
Buttons, Eye color, ...) and hand-moved single entries between them. Both are
needed: his ~240 hand moves alone cover only a sliver of each body (the 10-13
table read only those). bro_groups.json is the Studio's own final membership,
dumped by running the editor's load_all + rebuild_families on the cartridge
with his groups file, so it holds every entry's label as the Studio shows it.
The earlier hue-window classifier (lane BROCOLOR1) is gone.

THE ROWS. HAT = his Hat, UNDERSHIRT = his Undershirt (two picks since 11-13:
the two were one pick from 10-13 until Tango asked for them apart),
OVERALLS = Overalls, SHOES = Shoes. Everything else he labelled (Gloves,
Buttons, Eye color, white, Wario's jh4, Wario's Hat Blue and Hat white, which
are the W badge on his cap and its white edge, kept like Mario's M) and every
family he left unnamed keeps the cartridge colour. A seam entry the Studio lifts out of every family and
recolours by following one family (bro_groups.json seam_blends) joins the row
of the family it follows; bro_groups.json holds no two-family blend.

HELD BACK (lane BROSKIN1). bro_groups_keep.json lists entries that keep the
cartridge colour although his labels put them in a picker row, each with its
reason. It is empty today: Wario's bare-arm skin, which the Studio clustered
into his yellow shirt, now sits in his N/A label in the Studio itself. A listed
entry that is no longer in a picker label is an error, so the list cannot go
stale silently.

THE MAIN TONE. For each category in each palette the entry the most texels
read is written as `main`: the pick lands exactly there and the rest of the
category follows as a ramp (vs_char_palette_gen.h). kBroCart is the cartridge
colour of each bro's category, the main tone in the BODY file: the popup's
classic cells, and a pick equal to it leaves the palette untouched. The hat
has no entries in the body file, so its cartridge colour is the undershirt's
(the joined row's colour before the split), and both rows keep the classic
cell and the "no pick" test they had as one row.

Run from the worktree root:
    python port/tools/gen_vs_char_groups.py [--files extracted/dsd/files]
        [--labels port/tools/bro_groups.json] [--keep port/tools/bro_groups_keep.json]
        [--out port/hal/vs_char_groups.h] [--json <dump>]
"""
import argparse
import json
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lovesme_compose as lc  # noqa: E402

CHARS = ["mario", "luigi", "wario"]          # engine order: 0 Mario, 1 Luigi, 2 Wario
CAT_HAT, CAT_OVERALLS, CAT_GLOVES, CAT_BUTTONS, CAT_SHOES, CAT_UNDERSHIRT = 0, 1, 2, 3, 4, 5
CAT_COUNT = 6
CAT_NAMES = ["hat", "overalls", "gloves", "buttons", "shoes", "undershirt"]
# the table's order: the undershirt sorts with the hat, where it sat as one row, so every record keeps its place and kIdx_ number
CAT_RANK = {CAT_HAT: 0, CAT_UNDERSHIRT: 0, CAT_OVERALLS: 1, CAT_GLOVES: 2, CAT_BUTTONS: 3, CAT_SHOES: 4}
PICKED = (CAT_HAT, CAT_UNDERSHIRT, CAT_OVERALLS, CAT_SHOES)

# Tango's Studio label -> the picker row that recolours it; any other label keeps the cartridge colour
CAT_OF_LABEL = {"Hat": CAT_HAT, "Undershirt": CAT_UNDERSHIRT, "Overalls": CAT_OVERALLS, "Shoes": CAT_SHOES}

# arc0.narc member index, file path served to the game, is it the body file
FILES = {
    "mario": [(131, "data/player/mario_model.bmd", True), (129, "data/player/mario_head_cap.bmd", False),
              (130, "data/player/mario_head_nocap.bmd", False)],
    "luigi": [(128, "data/player/luigi_model.bmd", True), (126, "data/player/luigi_head_cap.bmd", False),
              (127, "data/player/luigi_head_nocap.bmd", False)],
    "wario": [(186, "data/player/wario_model.bmd", True), (184, "data/player/wario_head_cap.bmd", False),
              (185, "data/player/wario_head_nocap.bmd", False)],
}


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
            return m["texs"][ti]
    pairs = {"luigi_head_1_pl": "luigi_head_1", "luigi_body_2_pl": "luigi_body_tr_2"}
    for t in m["texs"]:
        if t["name"] == pairs.get(pal_name):
            return t
    return None


def texel_counts(m, pal, ncol):
    """texels that read each palette entry (a blended texel half to each end)"""
    cnt = [0.0] * ncol
    tex = texture_for(m, pal["name"])
    if tex is not None:
        _, _, refs = decode_refs(tex, ncol)
        for r in refs:
            for k, wt in r:
                cnt[k] += wt
    return cnt


# TEXTURE-SWAP TWINS (lane PALPOWER1). Luigi's body is drawn from luigi_body_tr_1 with luigi_body_1_pl, and the
# Power Flower's vanish look swaps it (data/player/L_tx_bodytrans.btp, a texture sequence on mat_body) to
# luigi_body_tr_2 with luigi_body_2_pl. The two textures are the same 64x64 picture encoded twice, texel for
# texel the same colours, but each indexes its own palette, so the Studio could not credit body_2's entries
# to any rig part and Tango never labelled them: without this, a picked colour reached his normal body and
# never the vanish one, and vanish Luigi showed the cartridge's green and blue. So a twin palette's entries
# take their label from the source palette: for every texel both textures draw, each twin entry the texel
# reads votes (with its weight) for the label of whichever source entry at that texel is nearest it in
# colour, and the label with the most votes wins. An entry no texel reaches keeps the label it had.
# A twin palette never sets kBroCart: the cartridge colour stays the source palette's main tone.
# (file, twin palette, twin texture, source palette, source texture)
SWAP_TWINS = {
    "luigi": [("data/player/luigi_model.bmd", "luigi_body_2_pl", "luigi_body_tr_2", "luigi_body_1_pl",
               "luigi_body_tr_1")],
}


def twin_labels(m, src_lab, twin_pal, twin_tex, src_pal, src_tex):
    """{twin entry: (label, "twin")} voted from src_lab through the shared picture"""
    def pal_words(name):
        p = [p for p in m["pals"] if p["name"] == name][0]
        return [struct.unpack_from("<H", p["data"], i * 2)[0] for i in range(len(p["data"]) // 2)]

    def tex(name):
        return [t for t in m["texs"] if t["name"] == name][0]

    def c5(v):
        return (v & 31, (v >> 5) & 31, (v >> 10) & 31)

    ws, wt = pal_words(src_pal), pal_words(twin_pal)
    w1, h1, r1 = decode_refs(tex(src_tex), len(ws))
    w2, h2, r2 = decode_refs(tex(twin_tex), len(wt))
    if (w1, h1) != (w2, h2):
        raise SystemExit("swap twin %s is %dx%d, its source %s %dx%d" % (twin_tex, w2, h2, src_tex, w1, h1))
    votes = {}
    for p in range(len(r1)):
        if not r1[p] or not r2[p]:
            continue
        for k, wgt in r2[p]:
            ck = c5(wt[k])
            near = min(r1[p], key=lambda e: (sum((a - b) ** 2 for a, b in zip(c5(ws[e[0]]), ck)), e[0]))[0]
            lab = src_lab.get(near, ("", ""))[0]
            v = votes.setdefault(k, {})
            v[lab] = v.get(lab, 0.0) + wgt
    return {k: (max(sorted(v), key=lambda x: v[x]), "twin") for k, v in votes.items()}


def load_labels(path):
    """bro -> {(file, pal): {index: (label, how)}}, and bro -> {"file:pal": length}"""
    with open(path) as f:
        doc = json.load(f)
    if doc.get("version") != 1:
        raise SystemExit("%s: unknown version %r" % (path, doc.get("version")))
    labels, lens = {}, {}
    for bro in CHARS:
        b = doc["bros"][bro]
        lab = {}
        for fam in b["families"]:
            for fpath, pals in fam["members"].items():
                for pname, idx in pals.items():
                    for i in idx:
                        key = (fpath, pname)
                        if i in lab.setdefault(key, {}):
                            raise SystemExit("%s %s %s[%d] is in two families" % (path, bro, pname, i))
                        lab[key][i] = (fam["name"], "family")
        for s in b["seam_blends"]:
            key = (s["file"], s["pal"])
            if s["index"] in lab.setdefault(key, {}):
                raise SystemExit("%s %s %s[%d] is a family member and a seam" % (path, bro, s["pal"], s["index"]))
            lab[key][s["index"]] = (s["follows"], "seam")
        if b.get("blend_pairs"):
            raise SystemExit("%s: two-family blends are not handled here" % path)
        labels[bro], lens[bro] = lab, b["pal_len"]
    return labels, lens


def load_keep(path):
    """bro -> {(file, pal): set of entries that stay cartridge}"""
    with open(path) as f:
        doc = json.load(f)
    if doc.get("version") != 1:
        raise SystemExit("%s: unknown version %r" % (path, doc.get("version")))
    keep = {}
    for k in doc["keep_cartridge"]:
        if k["bro"] not in CHARS or not k.get("why"):
            raise SystemExit("%s: bad entry %r" % (path, k))
        keep.setdefault(k["bro"], {}).setdefault((k["file"], k["pal"]), set()).update(k["index"])
    return keep


def rgb8(v):
    return (((v & 31) * 255 // 31) << 16) | ((((v >> 5) & 31) * 255 // 31) << 8) | (((v >> 10) & 31) * 255 // 31)


def collect(files_root, labels_path, keep_path):
    arc = os.path.join(files_root, "ARCHIVE", "arc0.narc")
    labels, lens = load_labels(labels_path)
    keep = load_keep(keep_path)
    kept = []
    records, dump = [], {}
    cart = [[0] * CAT_COUNT for _ in CHARS]
    cart_drawn = [[-1.0] * CAT_COUNT for _ in CHARS]
    for ci, chr_name in enumerate(CHARS):
        for idx, path, body in FILES[chr_name]:
            m = lc.parse_bmd(lc.narc_member(arc, idx))
            twins = set()
            for tfile, tpal, ttex, spal, stex in SWAP_TWINS.get(chr_name, []):
                if tfile != path:
                    continue
                derived = twin_labels(m, labels[chr_name].get((path, spal), {}), tpal, ttex, spal, stex)
                merged = dict(labels[chr_name].get((path, tpal), {}))
                merged.update(derived)
                labels[chr_name][(path, tpal)] = merged
                twins.add(tpal)
            for pal in m["pals"]:
                words = [struct.unpack_from("<H", pal["data"], i * 2)[0] for i in range(len(pal["data"]) // 2)]
                want = lens[chr_name].get("%s:%s" % (path, pal["name"]))
                if want != len(words):
                    raise SystemExit("%s %s: %d colours in the ROM, %r in the labels" % (path, pal["name"], len(words), want))
                if "eye" in pal["name"]:
                    continue
                if not pal["name"].startswith(chr_name):
                    raise SystemExit("arc0 member %d is not %s (palette %s)" % (idx, path, pal["name"]))
                lab = labels[chr_name].get((path, pal["name"]), {})
                cnt = texel_counts(m, pal, len(words))
                per = [lab.get(i, ("", ""))[0] for i in range(len(words))]
                how = [lab.get(i, ("", ""))[1] for i in range(len(words))]
                held = keep.get(chr_name, {}).get((path, pal["name"]), set())
                for i in sorted(held):
                    if i >= len(words) or per[i] not in CAT_OF_LABEL:
                        raise SystemExit("%s %s[%d] is held back but not in a picker label" % (path, pal["name"], i))
                    how[i] = "kept"
                    kept.append((ci, path, pal["name"], i))
                cats = [CAT_OF_LABEL.get(x, -1) if how[i] != "kept" else -1 for i, x in enumerate(per)]
                dump[pal["name"]] = {"file": path, "chr": ci, "labels": per, "how": how, "cats": cats, "cnt": cnt,
                                     "words": words}
                for label in sorted(set(x for x in per if x in CAT_OF_LABEL)):
                    members = [k for k, x in enumerate(per) if x == label and how[k] != "kept"]
                    c = CAT_OF_LABEL[label]
                    main = max(members, key=lambda k: (cnt[k], -k))
                    drawn = sum(cnt[k] for k in members)
                    records.append({"chr": ci, "cat": c, "label": label, "file": path, "pal": pal["name"],
                                    "idx": members, "seams": sum(1 for k in members if how[k] == "seam"),
                                    "main": main, "drawn": drawn})
                    if body and pal["name"] not in twins and drawn > cart_drawn[ci][c]:
                        cart_drawn[ci][c] = drawn
                        cart[ci][c] = rgb8(words[main])
    records.sort(key=lambda r: (r["chr"], CAT_RANK[r["cat"]], r["file"], r["pal"], r["label"]))
    for ci in range(len(CHARS)):
        if not cart[ci][CAT_HAT]:
            cart[ci][CAT_HAT] = cart[ci][CAT_UNDERSHIRT]
    for ci, chr_name in enumerate(CHARS):
        for (path, pname), held in keep.get(chr_name, {}).items():
            if not any(k[0] == ci and k[1] == path and k[2] == pname for k in kept):
                raise SystemExit("held-back palette %s %s not found" % (path, pname))
    return records, dump, cart, sorted(kept)


def emit(records, cart, kept):
    out = []
    w = out.append
    w("/* vs_char_groups.h -- GENERATED by port/tools/gen_vs_char_groups.py from Tango's")
    w(" * Studio labels (port/tools/bro_groups.json) and the extracted ROM (ARCHIVE/arc0.narc,")
    w(" * the nine bro BMDs). DO NOT EDIT BY HAND.")
    w(" *")
    w(" * The bros' category -> palette-entry membership: hat (his Hat label), undershirt")
    w(" * (Undershirt), overalls (Overalls), shoes (Shoes). Gloves and buttons keep their")
    w(" * slots for SM64DS_VS_CHAR_COLORS's grammar and are empty. The undershirt is the")
    w(" * last category (it was one pick with the hat until 11-13), so the grammar's old")
    w(" * five fields keep their places.")
    w(" * Each record is one labelled family's entries in one named palette; `main` is")
    w(" * the entry the most texels read (where a pick lands). kBroCart is each bro's")
    w(" * cartridge colour per category (the body file's main tone; the hat, which has no")
    w(" * body entries, takes the undershirt's). kVscKeepCart lists")
    w(" * the entries held back from every row (port/tools/bro_groups_keep.json).")
    w(" *")
    w(" * COVERAGE (label palette entries / texels drawn with them):")
    for ci, chr_name in enumerate(CHARS):
        for c in PICKED:
            rs = [r for r in records if r["chr"] == ci and r["cat"] == c]
            w(" *   %-6s %-15s %s" % (chr_name, CAT_NAMES[c], ", ".join(
                "%s %s %d/%d" % (r["label"], r["pal"], len(r["idx"]), int(r["drawn"] + 0.5)) for r in rs) or "-"))
    w(" */")
    w("#ifndef PORT_HAL_VS_CHAR_GROUPS_H")
    w("#define PORT_HAL_VS_CHAR_GROUPS_H")
    w("")
    w("namespace vscg {")
    w("")
    w("enum { CAT_HAT = 0, CAT_OVERALLS = 1, CAT_GLOVES = 2,")
    w("       CAT_BUTTONS = 3, CAT_SHOES = 4, CAT_UNDERSHIRT = 5, CAT_COUNT = 6 };")
    w("")
    w("struct Group {")
    w("    int         chr;    /* 0 Mario, 1 Luigi, 2 Wario */")
    w("    int         cat;    /* 0..5, the category above */")
    w("    const char *file;   /* served BMD path, e.g. data/player/mario_model.bmd */")
    w("    const char *pal;    /* palette name inside that BMD, e.g. mario_body_pl */")
    w("    int         count;  /* number of entries */")
    w("    const int  *idx;    /* the palette entries this category owns, ascending */")
    w("    int         main;   /* the most-drawn entry of them */")
    w("    const char *label;  /* Tango's Studio label these entries carry */")
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
        w('    { %d, %d, "%s", "%s", %d, kIdx_%d, %d, "%s" },' % (r["chr"], r["cat"], r["file"], r["pal"], len(r["idx"]),
                                                                  i, r["main"], r["label"]))
    w("};")
    w("static const int kVscGroupCount = %d;" % len(records))
    w("")
    w("/* entries that stay cartridge though a picker label holds them (bro_groups_keep.json) */")
    w("struct Kept {")
    w("    int         chr;")
    w("    const char *file;")
    w("    const char *pal;")
    w("    int         index;")
    w("};")
    w("")
    w("static const Kept kVscKeepCart[] = {")
    for k in kept:
        w('    { %d, "%s", "%s", %d },' % k)
    if not kept:
        w("    { -1, \"\", \"\", -1 },   /* none held back: a C++ array may not be empty */")
    w("};")
    w("static const int kVscKeepCartCount = %d;" % len(kept))
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
    ap.add_argument("--labels", default=os.path.join(here, "bro_groups.json"))
    ap.add_argument("--keep", default=os.path.join(here, "bro_groups_keep.json"))
    ap.add_argument("--out", default=os.path.normpath(os.path.join(here, "..", "hal", "vs_char_groups.h")))
    ap.add_argument("--json", default=None)
    a = ap.parse_args()
    records, dump, cart, kept = collect(a.files, a.labels, a.keep)
    for r in records:
        print("  %-6s %-15s %-10s %-16s %3d entries (%d seam)  main %3d  %6.0f texels" % (
            CHARS[r["chr"]], CAT_NAMES[r["cat"]], r["label"], r["pal"], len(r["idx"]), r["seams"], r["main"],
            r["drawn"]), file=sys.stderr)
    for ci, chr_name in enumerate(CHARS):
        for c in PICKED:
            if not cart[ci][c]:
                raise SystemExit("%s has no %s label in its body file" % (chr_name, CAT_NAMES[c]))
    with open(a.out, "w", encoding="ascii", newline="\n") as f:
        f.write(emit(records, cart, kept))
    if a.json:
        with open(a.json, "w") as f:
            json.dump(dump, f)
    print("wrote %s" % a.out, file=sys.stderr)


if __name__ == "__main__":
    main()
