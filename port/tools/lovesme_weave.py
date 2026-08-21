"""Weave mario_model_mg's character into yoshi_model.bmd, keeping the wall.

The Loves Me...? minigame's MG/yoshi_model.bmd is not just Yoshi: bone 1,
"polySurface689", is the castle wall the character sits on during the rules
card (materials mc_til_str/mc_tile00/tri_tile, palettes f_game1..3), baked
into the character file because the ROM only ever stars Yoshi there. The
LovesMeCharacter mod therefore cannot simply serve mario_model_mg.bmd -- the
wall goes with Yoshi. This tool builds the file hal/fs_mods.cpp prefers:
yoshi's file with the wall kept and the character replaced by Mario.

Layout facts derived from the real files + the decomp's BMD_File.h and
Model::UpdateFileOffsets (the authoritative list of every pointer the game
rebases on load):
  header 0x3c bytes (15 u32): scale, nBones, boneOff, nDL, dlOff, nTex, texOff,
    nPal, palOff, nMat, matOff, unk2c(ptr), unk30, unk34, +0x38 (unread)
  DL table: nDL x {u32 count, u32 subPtr}; sub = {u32 mapCount, u32 mapPtr,
    u32 streamSize, u32 streamPtr}; map entries are u8 transform ids
    (bone index - 1); streams are raw GX command data, position independent.
  bone 0x40: +0 idx(u32), +4 nameOff, +8/+A/+C parent/child/sibling (s16 rel),
    +0xE pad, +0x10 SRT block through +0x2F (scale u32 xyz, rot u16 xyz, pad,
    trans u32 xyz), +0x30 bindCount, +0x34 matIdsPtr, +0x38 dlIdsPtr, +0x3C flags
  material 0x30: +0 nameOff, +4 texIdx, +8 palIdx, rest render params
  texture 0x14: +0 nameOff, +4 dataPtr, +8 size, +C dims, +10 flags
  palette 0x10: +0 nameOff, +4 dataPtr, +8 size, +C flags

Merged file: yoshi's header/bones/tree/bindings/wall DLs+materials+textures+
palettes, with the character pieces (DL 3 body / DL 4 eye, mat 3/4, tex 3/4,
pal 3/4) replaced by mario's (his DL 0/1, mat 0/1 with tex/pal indices +3,
tex 0/1, pal 0/1, body+eye map bytes +1 for the wall bone shift), and the
character bones' SRT blocks (+0x10..+0x2F) replaced by mario's so the rest
pose is his. Mario's vertex data is divided by 16 on the way in (see
rescale_stream for why that and nothing else scales). All seventeen bones
stay, so the four yoshi_*.bca animations serve unmodified alongside this
file.
"""
import struct
import sys
import os


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


def parse(path):
    d = maybe_decompress(open(path, "rb").read())
    h = struct.unpack_from("<15I", d, 0)
    return d, h


def get_dl(d, dl_off, i):
    cnt, sub = struct.unpack_from("<II", d, dl_off + i * 8)
    assert cnt == 1, f"dl{i} count {cnt}"
    mc, mp, ss, sp = struct.unpack_from("<4I", d, sub)
    return {"map": bytearray(d[mp:mp + mc]), "stream": bytes(d[sp:sp + ss])}


# GX FIFO parameter-word counts for every command the DS accepts. The weave
# only ever rescales streams it can parse end to end; an unknown command is
# a hard refusal, never a guess.
GX_PARAMS = {0x00: 0, 0x10: 1, 0x11: 0, 0x12: 1, 0x13: 1, 0x14: 1, 0x15: 0,
             0x16: 16, 0x17: 12, 0x18: 16, 0x19: 12, 0x1a: 9, 0x1b: 3,
             0x1c: 3, 0x20: 1, 0x21: 1, 0x22: 1, 0x23: 2, 0x24: 1, 0x25: 1,
             0x26: 1, 0x27: 1, 0x28: 1, 0x29: 1, 0x2a: 1, 0x2b: 1, 0x30: 1,
             0x31: 1, 0x32: 1, 0x33: 1, 0x34: 32, 0x40: 1, 0x41: 0}


def rescale_stream(stream, shift):
    """Divide every vertex coordinate in a GX stream by 1 << shift.

    The header scale word multiplies STORED VERTEX DATA only -- bone and
    animation translations live in true units either way, which is why the
    two source files can share a skeleton pose while storing geometry
    sixteen to one apart. Mario's true-size vertices moving into yoshi's
    scale-4 file therefore shrink by 16 here and nowhere else.

    Handles the vertex commands 0x23 (xyz as three s16) and 0x25/0x26/0x27
    (two s16 components, the third carried over). Any command with another
    geometric meaning (matrix loads, MTX_TRANS, VTX_10, VTX_DIFF) is
    refused: neither source stream uses them today, and silently scaling
    half a matrix would be a wrong model, not an error message.
    """
    def scale16(v):
        # round half away from zero, the same rule as hal/fs_mods.cpp's
        # vtx_scale -- the two implementations are kept byte-identical
        if v >= 0x8000:
            v -= 0x10000
        half = 1 << (shift - 1)
        v = (v + half) >> shift if v >= 0 else -((-v + half) >> shift)
        return v & 0xffff

    out = bytearray(stream)
    p = 0
    end = len(out)
    while p + 4 <= end:
        cmds = out[p:p + 4]
        p += 4
        for c in cmds:
            n = GX_PARAMS.get(c)
            assert n is not None, f"unknown GX command {c:#x}"
            assert c not in (0x16, 0x17, 0x18, 0x19, 0x1a, 0x1c, 0x24, 0x28), \
                f"stream uses {c:#x}, which this rescale refuses"
            if c == 0x23:
                (p0, p1) = struct.unpack_from("<II", out, p)
                p0 = scale16(p0 & 0xffff) | (scale16(p0 >> 16) << 16)
                p1 = (p1 & 0xffff0000) | scale16(p1 & 0xffff)
                struct.pack_into("<II", out, p, p0, p1)
            elif c in (0x25, 0x26, 0x27):
                (p0,) = struct.unpack_from("<I", out, p)
                p0 = scale16(p0 & 0xffff) | (scale16(p0 >> 16) << 16)
                struct.pack_into("<I", out, p, p0)
            p += n * 4
    assert p == end, "stream length misaligned"
    return bytes(out)


def weave(yoshi_path, mario_path):
    yd, yh = parse(yoshi_path)
    md, mh = parse(mario_path)
    (y_scale, y_nb, y_bo, y_ndl, y_dlo, y_ntex, y_texo,
     y_npal, y_palo, y_nmat, y_mato, y_u2c, y_u30, y_u34, y_pad) = yh
    m_nb, m_bo = mh[1], mh[2]
    m_dlo, m_texo, m_palo, m_mato = mh[4], mh[6], mh[8], mh[10]
    assert y_nb == 17 and m_nb == 16 and y_ndl == 5

    # ---- gather DLs: wall = yoshi 0..2, body/eye = mario 0..1 (map +1) ----
    dls = [get_dl(yd, y_dlo, i) for i in range(3)]
    for i in range(2):
        dl = get_dl(md, m_dlo, i)
        dl["map"] = bytearray(b + 1 for b in dl["map"])
        assert max(dl["map"]) <= 16
        dl["stream"] = rescale_stream(dl["stream"], y_scale)
        dls.append(dl)

    # ---- bones: yoshi records, mario SRT on the character bones -----------
    bones = bytearray(yd[y_bo:y_bo + 17 * 0x40])
    for yi in range(2, 17):
        mi = yi - 1
        yr, mr = (yi) * 0x40, m_bo + mi * 0x40
        # names must agree or the splice is misaligned
        y_nm_off = struct.unpack_from("<I", bones, yr + 4)[0]
        m_nm_off = struct.unpack_from("<I", md, mr + 4)[0]
        y_nm = yd[y_nm_off:yd.index(b"\0", y_nm_off)]
        m_nm = md[m_nm_off:md.index(b"\0", m_nm_off)]
        assert y_nm == m_nm, (yi, y_nm, m_nm)
        bones[yr + 0x10:yr + 0x30] = md[mr + 0x10:mr + 0x30]

    # ---- the shared blob between bone table and texture table -------------
    # (bone names + binding lists + the unk2c target). Copied verbatim;
    # every yoshi pointer into it moves by one delta.
    y_blob_lo = y_bo + 17 * 0x40
    y_blob_hi = y_texo
    blob = yd[y_blob_lo:y_blob_hi]

    # one fresh name pool for every tex/pal/mat record, both source files
    pool = bytearray()

    def pooled(src_d, off):
        s = src_d[off:src_d.index(b"\0", off)] + b"\0"
        r = len(pool)
        pool.extend(s)
        return r

    # ---- records: textures / palettes / materials -------------------------
    def recs(d, off, n, size):
        return [bytearray(d[off + i * size:off + (i + 1) * size])
                for i in range(n)]
    y_tex, m_tex = recs(yd, y_texo, 5, 0x14), recs(md, m_texo, 2, 0x14)
    y_pal, m_pal = recs(yd, y_palo, 5, 0x10), recs(md, m_palo, 2, 0x10)
    y_mat, m_mat = recs(yd, y_mato, 5, 0x30), recs(md, m_mato, 2, 0x30)
    texs = y_tex[:3] + m_tex
    pals = y_pal[:3] + m_pal
    mats = y_mat[:3] + m_mat
    for r in mats[3:]:  # mario materials: tex/pal indices shift by +3
        t, p = struct.unpack_from("<II", r, 4)
        struct.pack_into("<II", r, 4, t + 3, p + 3)

    def data_of(i, r):
        src = yd if i < 3 else md
        (ptr,) = struct.unpack_from("<I", r, 4)
        (size,) = struct.unpack_from("<I", r, 8)
        return src[ptr:ptr + size]
    tex_data = [data_of(i, r) for i, r in enumerate(texs)]
    pal_data = [data_of(i, r) for i, r in enumerate(pals)]

    # ---- lay the file out --------------------------------------------------
    out = bytearray(0x3c)                     # header, filled last
    dl_table_off = len(out)
    out += b"\0" * (5 * 8)
    sub_offs = []
    for dl in dls:
        sub = len(out); out += b"\0" * 16
        mp = len(out); out += bytes(dl["map"])
        while len(out) % 4: out += b"\0"
        sp = len(out); out += dl["stream"]
        while len(out) % 4: out += b"\0"
        struct.pack_into("<4I", out, sub, len(dl["map"]), mp,
                         len(dl["stream"]), sp)
        sub_offs.append(sub)
    for i, so in enumerate(sub_offs):
        struct.pack_into("<II", out, dl_table_off + i * 8, 1, so)

    bone_off = len(out)
    out += bones
    blob_off = len(out)
    out += blob
    delta = blob_off - y_blob_lo

    # intern every record name now (source file per side), pool placed here
    for group in (texs, pals, mats):
        for i, r in enumerate(group):
            (nm,) = struct.unpack_from("<I", r, 0)
            struct.pack_into("<I", r, 0, pooled(yd if i < 3 else md, nm))
    pool_off = len(out)
    out += bytes(pool)
    while len(out) % 4: out += b"\0"

    tex_off = len(out); out += b"".join(bytes(r) for r in texs)
    pal_off = len(out); out += b"".join(bytes(r) for r in pals)
    mat_off = len(out); out += b"".join(bytes(r) for r in mats)
    data_off = []
    for blobs in (tex_data, pal_data):
        offs = []
        for b in blobs:
            offs.append(len(out)); out += b
            while len(out) % 4: out += b"\0"
        data_off.append(offs)

    # ---- fixups ------------------------------------------------------------
    # bone pointers: name/matIds/dlIds all lived in [y_blob_lo, y_blob_hi)
    for i in range(17):
        r = bone_off + i * 0x40
        for f in (4, 0x34, 0x38):
            (v,) = struct.unpack_from("<I", out, r + f)
            assert y_blob_lo <= v < y_blob_hi, (i, f, hex(v))
            struct.pack_into("<I", out, r + f, v + delta)
    # record name pointers: every one is a pool offset now
    for off, n, size in ((tex_off, 5, 0x14), (pal_off, 5, 0x10),
                         (mat_off, 5, 0x30)):
        for i in range(n):
            r = off + i * size
            (v,) = struct.unpack_from("<I", out, r)
            struct.pack_into("<I", out, r, pool_off + v)
    # texture/palette data pointers
    for j, (off, n, size) in enumerate(((tex_off, 5, 0x14),
                                        (pal_off, 5, 0x10))):
        for i in range(n):
            struct.pack_into("<I", out, off + i * size + 4, data_off[j][i])

    # ---- header ------------------------------------------------------------
    assert y_blob_lo <= y_u2c < y_blob_hi, hex(y_u2c)
    struct.pack_into("<15I", out, 0, y_scale, 17, bone_off, 5, dl_table_off,
                     5, tex_off, 5, pal_off, 5, mat_off, y_u2c + delta,
                     y_u30, y_u34, y_pad)
    return bytes(out)


def main():
    if len(sys.argv) not in (2, 4):
        print("usage: lovesme_weave.py <out.bmd> "
              "[<yoshi_model.bmd> <mario_model_mg.bmd>]\n"
              "       source paths default to extracted/dsd/files/MG under "
              "the repo root", file=sys.stderr)
        return 2
    if len(sys.argv) == 4:
        ysrc, msrc = sys.argv[2], sys.argv[3]
    else:
        root = os.path.dirname(os.path.dirname(
            os.path.dirname(os.path.abspath(__file__))))
        mg = os.path.join(root, "extracted", "dsd", "files", "MG")
        ysrc = os.path.join(mg, "yoshi_model.bmd")
        msrc = os.path.join(mg, "mario_model_mg.bmd")
    merged = weave(ysrc, msrc)
    open(sys.argv[1], "wb").write(merged)
    print(f"wrote {sys.argv[1]}: {len(merged)} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main())
