"""The Loves Me...? character composer: reference implementation.

Builds every LovesMeCharacter model the mod can serve, from the player's own
extraction, keeping the castle wall that lives inside MG/yoshi_model.bmd
(bone 1, polySurface689 -- see hal/fs_mods.cpp's header for why the wall is
in the character file at all). hal/fs_mods.cpp carries the C++ twin that
runs on player machines; THE TWO ARE KEPT BYTE-IDENTICAL, verified with
SM64DS_LOVESME_WEAVE_DUMP=<path> against this tool's output. This file
supersedes lovesme_weave.py (the fixed mario-only shape).

Characters and their sources:
  mario        MG/mario_model_mg.bmd (self-contained minigame model)
  yoshi_red    MG/yoshi_model.bmd with the body palette re-authored
  yoshi_blue     through the color-correspondence rows of arc0[196]'s
  yoshi_yellow   stacked yoshi_all_16p_pl (green/red/blue/yellow VS rows)
  luigi        arc0[128] body + arc0[126] cap head (mat_head retargeted to
               luigi_head_1: the file's own index names head_2, a
               runtime-swapped variant that decodes to noise)
  wario        arc0[186] body + arc0[184] head

Metal Wario (data/player/wario_metal_model.bmd) composed cleanly and was
CUT on looks: without the env-map material path he renders as a gold
statue, and the owner ruled him out.

FORMAT FACTS, all derived from the real files plus the decomp's matched
code (BMD_File.h, Model::UpdateFileOffsets, Model::LoadTexAndPal,
func_020462d0):

  header 0x3c: scale, nBones, boneOff, nDL, dlOff, nTex, texOff, nPal,
    palOff, nMat, matOff, slotTableOff(+0x2c), unk30, unk34, +0x38(unread)
  +0x2c -> the TRANSFORM SLOT TABLE: u16 per slot naming the bone that
    slot's matrix comes from. DL map bytes are SLOT numbers and each file
    numbers only its geometry-owning bones (wario_metal's table starts at
    bone 2), so maps never move between files without translating
    src slot -> src bone -> merged bone -> merged slot.
  bone 0x40: +0 idx, +4 nameOff, +8/+A/+C parent/child/sibling s16 rel,
    +0x10..+0x2F rest SRT, +0x30 bindCount, +0x34 matIdsPtr, +0x38
    dlIdsPtr (count u8s each, dls after mats), +0x3C flags
  material 0x30: +0 nameOff, +4 texIdx, +8 palIdx, rest render params
  texture 0x14: +0 nameOff, +4 dataPtr, +8 size, +C dims, +10 flags
    (format bits 26-28; 0x20000000 = color 0 transparent)
  palette 0x10: +0 nameOff, +4 dataPtr, +8 size, +C vram scratch
  4x4-compressed textures (format 5) carry a palette-index companion at
    dataPtr + size, half the main size (Model::LoadTexAndPal passes
    t->data + sz). The flower scene's VRAM cursors never budget the
    compressed format, so ANY compressed upload stomps the scene's regular
    texture VRAM -- every format-5 texture is transcoded to plain 256color
    here. LoadTexAndPal uploads unreferenced table entries too, so ALL of
    them transcode, paired material-first, name+"_pl" second, positional
    within the source file third.
  GX streams are position independent; the header scale word multiplies
    STORED VERTEX DATA only, so a piece's vertices divide by
    1 << (4 - its scale) on the way into yoshi's scale-4 file.
"""
import struct
import sys
import os

PLAYER_BONES = ["skl_root", "leg_l1", "leg_l2", "ankle_l1", "leg_r1",
                "leg_r2", "ankle_r1", "spin", "arm_l1", "arm_l2", "wrist_l1",
                "arm_r1", "arm_r2", "wrist_r1", "face_1"]


def maybe_decompress(data):
    if data[:4] == b"LZ77":
        data = data[4:]
    if len(data) >= 4 and data[0] == 0x10:
        out_len = data[1] | (data[2] << 8) | (data[3] << 16)
        out = bytearray()
        i = 4
        while len(out) < out_len and i < len(data):
            flags = data[i]; i += 1
            for bit in range(8):
                if len(out) >= out_len or i >= len(data):
                    break
                if flags & (0x80 >> bit):
                    b1, b2 = data[i], data[i + 1]; i += 2
                    length = (b1 >> 4) + 3
                    disp = ((b1 & 0xF) << 8 | b2) + 1
                    for _ in range(length):
                        out.append(out[-disp])
                else:
                    out.append(data[i]); i += 1
        return bytes(out)
    return data


def narc_member(path, idx):
    d = open(path, "rb").read()
    assert d[:4] == b"NARC"
    off = 0x10
    fat = []
    img = None
    while off < len(d):
        tag = d[off:off + 4]
        (size,) = struct.unpack_from("<I", d, off + 4)
        if tag == b"BTAF":
            (n,) = struct.unpack_from("<I", d, off + 8)
            for i in range(n):
                fat.append(struct.unpack_from("<II", d, off + 12 + i * 8))
        elif tag == b"GMIF":
            img = off + 8
        off += size
    s, e = fat[idx]
    return maybe_decompress(d[img + s:img + e])


def cstr(d, off):
    return d[off:d.index(b"\0", off)].decode()


def parse_bmd(d):
    h = struct.unpack_from("<15I", d, 0)
    scale, nb, bo, ndl, dlo, ntex, texo, npal, palo, nmat, mato = h[:11]
    m = {"scale": scale, "bones": [], "dls": [], "texs": [], "pals": [],
         "mats": []}
    for i in range(nb):
        rec = bytearray(d[bo + i * 0x40:bo + (i + 1) * 0x40])
        (nm,) = struct.unpack_from("<I", rec, 4)
        cnt = struct.unpack_from("<I", rec, 0x30)[0]
        (mi,) = struct.unpack_from("<I", rec, 0x34)
        (di,) = struct.unpack_from("<I", rec, 0x38)
        m["bones"].append({"rec": rec, "name": cstr(d, nm),
                           "mats": list(d[mi:mi + cnt]),
                           "dls": list(d[di:di + cnt])})
    for i in range(ndl):
        cnt, sub = struct.unpack_from("<II", d, dlo + i * 8)
        assert cnt == 1
        mc, mp, ss, sp = struct.unpack_from("<4I", d, sub)
        m["dls"].append({"map": bytearray(d[mp:mp + mc]),
                         "stream": bytearray(d[sp:sp + ss])})
    for key, off, n, size in (("texs", texo, ntex, 0x14),
                              ("pals", palo, npal, 0x10),
                              ("mats", mato, nmat, 0x30)):
        for i in range(n):
            rec = bytearray(d[off + i * size:off + (i + 1) * size])
            (nm,) = struct.unpack_from("<I", rec, 0)
            e = {"rec": rec, "name": cstr(d, nm)}
            if key != "mats":
                dp, sz = struct.unpack_from("<II", rec, 4)
                blob = sz
                if key == "texs":
                    (flags,) = struct.unpack_from("<I", rec, 0x10)
                    if ((flags >> 26) & 7) == 5:
                        blob = sz + sz // 2
                e["data"] = bytes(d[dp:dp + blob])
                e["size_field"] = sz
            m[key].append(e)
    max_slot = max((max(dl["map"]) for dl in m["dls"] if dl["map"]),
                   default=-1)
    m["slots"] = [struct.unpack_from("<H", d, h[11] + s * 2)[0]
                  for s in range(max_slot + 1)]
    for s in m["slots"]:
        assert s < nb, (s, nb)
    return m


def transcode_compressed(tex, pal):
    """Rewrite a 4x4-compressed texture entry as plain 256color."""
    (flags,) = struct.unpack_from("<I", tex["rec"], 0x10)
    assert ((flags >> 26) & 7) == 5
    w = 8 << ((flags >> 20) & 7)
    h = 8 << ((flags >> 23) & 7)
    main = tex["data"][:tex["size_field"]]
    comp = tex["data"][tex["size_field"]:]
    pdata = pal["data"]

    def color(off):
        if off + 2 > len(pdata):
            return 0
        return struct.unpack_from("<H", pdata, off)[0]

    def mix(a, b, na, nb, div):
        return ((((a & 31) * na + (b & 31) * nb) // div)
                | (((((a >> 5) & 31) * na + ((b >> 5) & 31) * nb) // div) << 5)
                | (((((a >> 10) & 31) * na + ((b >> 10) & 31) * nb) // div)
                   << 10))

    texels = [[0] * w for _ in range(h)]
    blocks_w = w // 4
    for bi in range(len(main) // 4):
        (block,) = struct.unpack_from("<I", main, bi * 4)
        (info,) = struct.unpack_from("<H", comp, bi * 2)
        base = (info & 0x3fff) * 4
        mode = info >> 14
        c0, c1 = color(base), color(base + 2)
        if mode == 0:
            cols = [c0, c1, color(base + 4), None]
        elif mode == 1:
            cols = [c0, c1, mix(c0, c1, 1, 1, 2), None]
        elif mode == 2:
            cols = [c0, c1, color(base + 4), color(base + 6)]
        else:
            cols = [c0, c1, mix(c0, c1, 5, 3, 8), mix(c0, c1, 3, 5, 8)]
        bx, by = (bi % blocks_w) * 4, (bi // blocks_w) * 4
        for t in range(16):
            texels[by + t // 4][bx + t % 4] = cols[(block >> (t * 2)) & 3]

    palette = []
    transparent = any(v is None for row in texels for v in row)
    if transparent:
        palette.append(0)
    index_of = {}
    out = bytearray()
    for row in texels:
        for v in row:
            if v is None:
                out.append(0)
                continue
            if v not in index_of:
                if len(palette) >= 256:
                    best, bd = 1 if transparent else 0, 1 << 30
                    for idx, c in enumerate(palette):
                        if transparent and idx == 0:
                            continue
                        d = (((v & 31) - (c & 31)) ** 2
                             + (((v >> 5) & 31) - ((c >> 5) & 31)) ** 2
                             + (((v >> 10) & 31) - ((c >> 10) & 31)) ** 2)
                        if d < bd:
                            best, bd = idx, d
                    index_of[v] = best
                else:
                    index_of[v] = len(palette)
                    palette.append(v)
            out.append(index_of[v])

    tex["data"] = bytes(out)
    tex["size_field"] = len(out)
    new_flags = (flags & ~((7 << 26) | 0x20000000)) | (4 << 26)
    if transparent:
        new_flags |= 0x20000000
    struct.pack_into("<I", tex["rec"], 0x10, new_flags)
    pal["data"] = b"".join(struct.pack("<H", c) for c in palette)
    while len(pal["data"]) % 16:
        pal["data"] += b"\0\0"
    pal["size_field"] = len(pal["data"])


GX_PARAMS = {0x00: 0, 0x10: 1, 0x11: 0, 0x12: 1, 0x13: 1, 0x14: 1, 0x15: 0,
             0x16: 16, 0x17: 12, 0x18: 16, 0x19: 12, 0x1a: 9, 0x1b: 3,
             0x1c: 3, 0x20: 1, 0x21: 1, 0x22: 1, 0x23: 2, 0x24: 1, 0x25: 1,
             0x26: 1, 0x27: 1, 0x28: 1, 0x29: 1, 0x2a: 1, 0x2b: 1, 0x30: 1,
             0x31: 1, 0x32: 1, 0x33: 1, 0x34: 32, 0x40: 1, 0x41: 0}
REFUSE = {0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x24, 0x28}


def rescale_stream(s, shift):
    if shift <= 0:
        assert shift == 0, "source stored larger than the wall file"
        return
    half = 1 << (shift - 1)

    def sc(raw):
        v = raw - 0x10000 if raw >= 0x8000 else raw
        v = (v + half) >> shift if v >= 0 else -((-v + half) >> shift)
        return v & 0xffff

    p = 0
    while p + 4 <= len(s):
        cmds = s[p:p + 4]
        p += 4
        for c in cmds:
            n = GX_PARAMS[c]
            assert c not in REFUSE, f"stream uses {c:#x}"
            if c == 0x23:
                (w0, w1) = struct.unpack_from("<II", s, p)
                w0 = sc(w0 & 0xffff) | (sc(w0 >> 16) << 16)
                w1 = (w1 & 0xffff0000) | sc(w1 & 0xffff)
                struct.pack_into("<II", s, p, w0, w1)
            elif c in (0x25, 0x26, 0x27):
                (w0,) = struct.unpack_from("<I", s, p)
                w0 = sc(w0 & 0xffff) | (sc(w0 >> 16) << 16)
                struct.pack_into("<I", s, p, w0)
            p += n * 4
    assert p == len(s)


def compose(yoshi, body, head=None):
    """The wall from yoshi's file plus a character: body (16-bone) and an
    optional 1-bone head bound to face_1. Returns merged .bmd bytes."""
    assert [b["name"] for b in body["bones"]][1:] == PLAYER_BONES, \
        [b["name"] for b in body["bones"]]
    assert yoshi["scale"] == 4

    dls, texs, pals, mats = ([yoshi["dls"][i] for i in range(3)],
                             yoshi["texs"][:3], yoshi["pals"][:3],
                             yoshi["mats"][:3])
    piece_off = []
    for piece in [body] + ([head] if head else []):
        piece_off.append((len(dls), len(texs), len(pals), len(mats)))
        shift = 4 - piece["scale"]
        for dl in piece["dls"]:
            nd = {"map": bytearray(dl["map"]),
                  "stream": bytearray(dl["stream"])}
            if piece is head:
                nd["map"] = bytearray([15])
            else:
                # merged map byte = merged bone - 1 (the merged slot table
                # is identity: slot s -> bone s + 1), and merged bone =
                # source bone + 1 for the wall, so the byte IS the source
                # bone the piece's own slot table names
                nd["map"] = bytearray(piece["slots"][slot]
                                      for slot in nd["map"])
            rescale_stream(nd["stream"], shift)
            dls.append(nd)
        for k, te in enumerate(piece["texs"]):
            if len(piece["texs"]) == len(piece["pals"]):
                te["pair_pal"] = piece["pals"][k]
        texs += piece["texs"]
        pals += piece["pals"]
        for mt in piece["mats"]:
            rec = bytearray(mt["rec"])
            t, p = struct.unpack_from("<II", rec, 4)
            struct.pack_into("<II", rec, 4, t + piece_off[-1][1],
                             p + piece_off[-1][2])
            mats.append({"rec": rec, "name": mt["name"]})

    pal_by_name = {e["name"]: e for e in pals}
    tex_pal = {}
    for mt in mats[3:]:
        t, p = struct.unpack_from("<II", mt["rec"], 4)
        tex_pal[t] = pals[p]
    for t, tex in enumerate(texs):
        (flags,) = struct.unpack_from("<I", tex["rec"], 0x10)
        if ((flags >> 26) & 7) != 5 or tex.get("transcoded"):
            continue
        pal = (tex_pal.get(t) or pal_by_name.get(tex["name"] + "_pl")
               or tex.get("pair_pal"))
        assert pal is not None, f"no palette pairing for {tex['name']}"
        transcode_compressed(tex, pal)
        tex["transcoded"] = True

    bones = []
    for i in range(17):
        y = yoshi["bones"][i]
        b = {"rec": bytearray(y["rec"]), "name": y["name"],
             "mats": [], "dls": []}
        if i == 1:
            b["mats"], b["dls"] = list(y["mats"]), list(y["dls"])
        else:
            src = body["bones"][0 if i == 0 else i - 1]
            b["rec"][0x10:0x30] = src["rec"][0x10:0x30]
            b["rec"][0x3c:0x40] = src["rec"][0x3c:0x40]
            mb, db = piece_off[0][3], piece_off[0][0]
            b["mats"] = [m + mb for m in src["mats"]]
            b["dls"] = [d + db for d in src["dls"]]
        bones.append(b)
    bones[0]["rec"][0x10:0x30] = yoshi["bones"][0]["rec"][0x10:0x30]
    if head:
        mb, db = piece_off[1][3], piece_off[1][0]
        hr = head["bones"][0]
        bones[16]["mats"] += [m + mb for m in hr["mats"]]
        bones[16]["dls"] += [d + db for d in hr["dls"]]

    out = bytearray(0x3c)
    dl_table = len(out)
    out += b"\0" * (len(dls) * 8)
    for i, dl in enumerate(dls):
        sub = len(out); out += b"\0" * 16
        mp = len(out); out += bytes(dl["map"])
        while len(out) % 4: out += b"\0"
        sp = len(out); out += bytes(dl["stream"])
        while len(out) % 4: out += b"\0"
        struct.pack_into("<4I", out, sub, len(dl["map"]), mp,
                         len(dl["stream"]), sp)
        struct.pack_into("<II", out, dl_table + i * 8, 1, sub)

    bone_off = len(out)
    out += b"\0" * (17 * 0x40)

    name_off = {}
    for b in bones:
        if b["name"] not in name_off:
            name_off[b["name"]] = len(out)
            out += b["name"].encode() + b"\0"
    binds = []
    for b in bones:
        assert len(b["mats"]) == len(b["dls"])
        if b["mats"]:
            binds.append((len(out), len(out) + len(b["mats"])))
            out += bytes(b["mats"]) + bytes(b["dls"])
        else:
            binds.append(None)
    dummy = len(out)
    out += b"\0"
    while len(out) % 2: out += b"\0"
    unk2c = len(out)
    for s in range(16):
        out += struct.pack("<H", s + 1)
    while len(out) % 4: out += b"\0"

    for i, b in enumerate(bones):
        rec = bytearray(b["rec"])
        struct.pack_into("<I", rec, 0, i)
        struct.pack_into("<I", rec, 4, name_off[b["name"]])
        struct.pack_into("<I", rec, 0x30, len(b["mats"]))
        mi, di = binds[i] if binds[i] else (dummy, dummy)
        struct.pack_into("<I", rec, 0x34, mi)
        struct.pack_into("<I", rec, 0x38, di)
        out[bone_off + i * 0x40:bone_off + (i + 1) * 0x40] = rec

    pool = {}
    for group in (texs, pals, mats):
        for e in group:
            if e["name"] not in pool:
                pool[e["name"]] = len(out)
                out += e["name"].encode() + b"\0"
    while len(out) % 4: out += b"\0"

    offs = {}
    for key, group, size in (("tex", texs, 0x14), ("pal", pals, 0x10),
                             ("mat", mats, 0x30)):
        offs[key] = len(out)
        for e in group:
            rec = bytearray(e["rec"])
            struct.pack_into("<I", rec, 0, pool[e["name"]])
            out += rec
    for key, group, size in (("tex", texs, 0x14), ("pal", pals, 0x10)):
        for i, e in enumerate(group):
            rec = offs[key] + i * size
            struct.pack_into("<I", out, rec + 4, len(out))
            struct.pack_into("<I", out, rec + 8, e["size_field"])
            out += e["data"]
            while len(out) % 4: out += b"\0"

    struct.pack_into("<15I", out, 0, 4, 17, bone_off, len(dls), dl_table,
                     len(texs), offs["tex"], len(pals), offs["pal"],
                     len(mats), offs["mat"], unk2c, 0, 0, 0)
    return bytes(out)


def palette_named(bmd, name):
    h = struct.unpack_from("<15I", bmd, 0)
    n_pal, pal_off = h[7], h[8]
    for i in range(n_pal):
        rec = pal_off + i * 0x10
        nm_off, data, size = struct.unpack_from("<III", bmd, rec)
        if cstr(bmd, nm_off) == name:
            return data, size
    raise KeyError(name)


def recolor_yoshi(mg, arc_yoshi, target_row):
    """MG yoshi bytes with yoshi_all_pl mapped green -> target through the
    stacked VS rows: nearest green-row color, brightness offset carried."""
    pdata, psize = palette_named(arc_yoshi, "yoshi_all_16p_pl")
    assert psize == 128
    rows = [[struct.unpack_from("<H", arc_yoshi, pdata + r * 32 + i * 2)[0]
             for i in range(16)] for r in range(4)]
    green, target = rows[0], rows[target_row]

    out = bytearray(mg)
    data, size = palette_named(mg, "yoshi_all_pl")
    assert size == 32
    for i in range(16):
        (c,) = struct.unpack_from("<H", out, data + i * 2)
        cr, cg, cb = c & 31, (c >> 5) & 31, (c >> 10) & 31
        best, bd = 0, 1 << 30
        for j in range(16):
            g = green[j]
            d2 = ((cr - (g & 31)) ** 2 + (cg - ((g >> 5) & 31)) ** 2
                  + (cb - ((g >> 10) & 31)) ** 2)
            if d2 < bd:
                best, bd = j, d2
        if green[best] == target[best]:
            continue
        g, t = green[best], target[best]
        nr = max(0, min(31, (t & 31) + cr - (g & 31)))
        ng = max(0, min(31, ((t >> 5) & 31) + cg - ((g >> 5) & 31)))
        nb = max(0, min(31, ((t >> 10) & 31) + cb - ((g >> 10) & 31)))
        struct.pack_into("<H", out, data + i * 2, nr | (ng << 5) | (nb << 10))
    return bytes(out)


def build(character, files_root):
    def load(rel):
        return parse_bmd(maybe_decompress(
            open(os.path.join(files_root, rel), "rb").read()))

    def load_arc0(idx):
        return parse_bmd(narc_member(
            os.path.join(files_root, "ARCHIVE", "arc0.narc"), idx))

    if character.startswith("yoshi_"):
        row = {"yoshi_red": 1, "yoshi_blue": 2, "yoshi_yellow": 3}[character]
        mg = maybe_decompress(
            open(os.path.join(files_root, "MG", "yoshi_model.bmd"),
                 "rb").read())
        arc = narc_member(os.path.join(files_root, "ARCHIVE", "arc0.narc"),
                          196)
        return recolor_yoshi(mg, arc, row)

    yoshi = load(os.path.join("MG", "yoshi_model.bmd"))
    if character == "mario":
        return compose(yoshi, load(os.path.join("MG", "mario_model_mg.bmd")))
    if character == "luigi":
        h = load_arc0(126)
        for e in h["mats"]:
            if e["name"] == "mat_head":
                struct.pack_into("<II", e["rec"], 4, 5, 5)
        return compose(yoshi, load_arc0(128), head=h)
    if character == "wario":
        return compose(yoshi, load_arc0(186), head=load_arc0(184))
    raise KeyError(character)


CHARACTERS = ["mario", "yoshi_red", "yoshi_blue", "yoshi_yellow", "luigi",
              "wario"]


def main():
    if len(sys.argv) not in (2, 3):
        print("usage: lovesme_compose.py <out_dir> [<character>|all]\n"
              "       sources come from extracted/dsd/files under the repo "
              "root", file=sys.stderr)
        return 2
    root = os.path.dirname(os.path.dirname(
        os.path.dirname(os.path.abspath(__file__))))
    files_root = os.path.join(root, "extracted", "dsd", "files")
    which = sys.argv[2] if len(sys.argv) == 3 else "all"
    names = CHARACTERS if which == "all" else [which]
    os.makedirs(sys.argv[1], exist_ok=True)
    for name in names:
        data = build(name, files_root)
        dst = os.path.join(sys.argv[1], f"{name}.bmd")
        open(dst, "wb").write(data)
        print(f"wrote {dst}: {len(data)} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main())
