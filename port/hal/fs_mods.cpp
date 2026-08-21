// Asset-swap mods: the file layer's half of the launcher's Mods panel.
//
// WHAT THIS IS. A mod here is a substitution at the SharedFilePtr seam: when
// the game asks the catalog for a file, it is handed a different file, or the
// same file with a byte transform applied after decompression. The game's own
// code -- src/, the overlays, everything byte-gated -- never learns the swap
// happened; it plays whatever bytes the seam serves, exactly as it plays the
// ROM's. Nothing here ships any asset: every byte served still comes out of
// the player's own extraction.
//
// WHY A SEPARATE FILE FROM hal/fs.cpp. fs.cpp links into fifteen targets and
// most of them are smoke tests with no hal/host_settings.cpp beside them, so
// fs.cpp cannot name a settings accessor without breaking their links. It
// carries two null function-pointer hooks instead (port_fs_mod_map /
// port_fs_mod_filter) and this file, linked only where host_settings.cpp
// already is, installs them from a static initializer. A target without this
// file gets null hooks and the exact fs.cpp behaviour it always had.
//
// BOOT-LATCHED, LIKE EVERY MOD. host_settings.h promises that only the
// screen-gap keys and Volume reload live and that the Mods panel means a
// restart. This one is the clearest case of why: the fs cache keys master
// copies by file id, so a mid-run flip would serve half old bytes and half
// new. The setting is read once, on the first load the hooks see.
//
// ---- THE ONE MOD SO FAR: the Loves Me...? character swap -------------------
//
// dScMgFlower_c (hal/scene_mg_flower.cpp, scene 390) stars the minigame
// archive's Yoshi: MG/yoshi_model.bmd and the four animations
// MG/yoshi_{wait,pick,like,hate}.bca, all loaded by FAT file id through
// SharedFilePtr. LovesMeCharacter=mario replaces him with the archive's own
// minigame Mario, MG/mario_model_mg.bmd.
//
// The two rigs were compared bone table to bone table before this was
// written (2026-08-21): every playable character's model in the ROM carries
// the identical 16-bone skeleton, same names in the same order --
//
//     *_all_root skl_root leg_l1 leg_l2 ankle_l1 leg_r1 leg_r2 ankle_r1
//     spin arm_l1 arm_l2 wrist_l1 arm_r1 arm_r2 wrist_r1 face_1
//
// -- and yoshi_model.bmd alone is that rig PLUS one extra mesh node,
// "polySurface689", spliced in at bone index 1. That node is not part of
// Yoshi: it is the CASTLE WALL the character sits on during the rules card,
// three tiled materials (mc_til_str/mc_tile00/tri_tile, palettes named
// f_game1..3) baked into the character file because the ROM only ever stars
// Yoshi here. Two serving modes follow from that:
//
//   OVERRIDE -- build/assets/mods/lovesme_mario.bmd, when present, is served
//   verbatim for the model id. Nothing generates this file in a normal
//   install; it exists so a custom model can be dropped in by hand and so
//   tools/lovesme_weave.py's output can be tested against the build below.
//
//   PREFERRED -- the WOVEN MODEL, built IN MEMORY on first ask from the two
//   catalog files themselves: yoshi_model.bmd with the wall kept and the
//   character display lists, materials, textures, palettes and bone rest
//   poses replaced by Mario's. weave_build() below is the C++ twin of
//   tools/lovesme_weave.py (the reference implementation; keep them in
//   step), and its header carries the whole file-format derivation. It
//   keeps all SEVENTEEN bones, so the four animations serve UNTOUCHED, and
//   because it is built from the player's own extraction at run time, a
//   shipped bundle needs no extra file and no launcher-side generator.
//
//   FALLBACK -- when the weave refuses (a source file not the shape the
//   derivation pinned), serve mario_model_mg.bmd directly for the model id.
//   The wall vanishes (known cosmetic gap), and because a .bca drives bones
//   BY INDEX, the filter drops descriptor row 1 (the wall bone's track)
//   from each animation so the 16-bone Mario gets the 16 rows that are his,
//   in his order.
//
// WHY DROPPING A ROW IS SAFE, derived from the files rather than assumed:
// a .bca is a 0x18-byte header (u16 boneCount, u16 frameCount, u32 looped,
// u32 scale/rotation/translation pool offsets, u32 descriptor table offset),
// then the three value pools, then boneCount 36-byte descriptor rows at the
// tail -- descOff + boneCount*36 == file size exactly, verified on all four
// files. Rows hold {flags, pool index} pairs and no bone number, and pool
// indices point backward into pools that do not move, so removing one row
// and decrementing the count leaves every surviving reference intact.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" int host_setting_lovesme_character(void);   /* hal/host_settings.cpp */
extern "C" const char *port_fs_catalog_path(unsigned); /* hal/fs.cpp */

extern "C" {
extern unsigned (*port_fs_mod_map)(unsigned fileID);
extern u32 (*port_fs_mod_filter)(unsigned fileID, u8 **data, u32 size);
}

namespace {

enum { CATALOG_SCAN_MAX = 4096, ANIM_COUNT = 4 };

/* File ids resolved by path, not hardcoded: files.tsv is generated from the
   player's own ROM, so the ids arrive already right for that dump. */
int g_resolved;
unsigned g_yoshi_model, g_mario_model;
unsigned g_anims[ANIM_COUNT];

int path_ends_with(const char *path, const char *suffix)
{
    const size_t pl = strlen(path), sl = strlen(suffix);
    return pl >= sl && memcmp(path + pl - sl, suffix, sl) == 0;
}

void resolve_ids(void)
{
    static const char *const kAnimNames[ANIM_COUNT] = {
        "MG/yoshi_wait.bca", "MG/yoshi_pick.bca",
        "MG/yoshi_like.bca", "MG/yoshi_hate.bca",
    };
    if (g_resolved)
        return;
    g_resolved = 1;
    for (unsigned id = 0; id < CATALOG_SCAN_MAX; ++id) {
        const char *p = port_fs_catalog_path(id);
        if (!p)
            continue;
        if (path_ends_with(p, "MG/yoshi_model.bmd"))
            g_yoshi_model = id;
        else if (path_ends_with(p, "MG/mario_model_mg.bmd"))
            g_mario_model = id;
        else
            for (int i = 0; i < ANIM_COUNT; ++i)
                if (path_ends_with(p, kAnimNames[i]))
                    g_anims[i] = id;
    }
    if (!g_yoshi_model || !g_mario_model) {
        fprintf(stderr, "[mods] LovesMeCharacter: MG models not in the "
                        "catalog; the swap is off for this run\n");
        return;
    }
    fprintf(stderr, "[mods] LovesMeCharacter mario: serving file %u for %u "
                    "and re-rigging %d animations\n",
            g_mario_model, g_yoshi_model, ANIM_COUNT);
}

/* The boot latch: -1 until first asked, then the setting as it was then. */
int lovesme_on(void)
{
    static int v = -1;
    if (v < 0)
        v = host_setting_lovesme_character() == 1;
    return v;
}

/* The woven model bytes, built or loaded whole on first ask. woven_state:
   0 not yet tried, 1 have bytes, -1 no bytes (fall back to the direct
   swap). */
u8 *g_woven;
long g_woven_len;
int g_woven_state;

#ifndef PORT_REPO_ROOT
#define PORT_REPO_ROOT "."
#endif

const char *mods_root(void)
{
    const char *env = getenv("SM64DS_ASSET_ROOT");
    return env && *env ? env : PORT_REPO_ROOT;
}

extern "C" void DecompressLZ16(void *src, void *dst); /* hal/fs.cpp */

/* A catalog file's decompressed bytes, malloc'd, or 0. The same read +
   LZ77-sniff hal/fs.cpp's cache fill does, kept separate so the weave can
   read its two SOURCE files without touching the cache it is about to
   feed. */
u8 *catalog_file_load(unsigned fileID, u32 *len_out)
{
    char path[520];
    const char *rel = port_fs_catalog_path(fileID);
    FILE *f;
    long fsize;
    u8 *raw, *out;
    if (!rel)
        return 0;
    snprintf(path, sizeof path, "%s/extracted/dsd/files/%s", mods_root(), rel);
    f = fopen(path, "rb");
    if (!f)
        return 0;
    fseek(f, 0, SEEK_END);
    fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    raw = (u8 *)malloc(fsize ? (size_t)fsize : 1);
    if (!raw || (long)fread(raw, 1, (size_t)fsize, f) != fsize) {
        fclose(f);
        free(raw);
        return 0;
    }
    fclose(f);
    if (fsize > 8 && memcmp(raw, "LZ77", 4) == 0) {
        u32 dec = (raw[4] | raw[5] << 8 | raw[6] << 16 | (u32)raw[7] << 24)
                  >> 8;
        out = (u8 *)malloc(dec ? dec : 1);
        if (out)
            DecompressLZ16(raw + 4, out);
        free(raw);
        *len_out = dec;
        return out;
    }
    *len_out = (u32)fsize;
    return raw;
}

/* ---- weave_build: the in-memory twin of tools/lovesme_weave.py -----------
   That tool's docstring carries the file-format derivation (header shape,
   record strides, which fields Model::UpdateFileOffsets rebases); this is a
   line-for-line translation and the two MUST stay in step -- the tool's
   output and this build are byte-identical by test, which is what keeps the
   override path honest. Every structural surprise lands on `refuse`, which
   the caller answers with the direct-swap fallback: a wrong-shaped source
   file costs the wall, never the game. */

u32 rd32(const u8 *d, u32 off) {
    return d[off] | d[off + 1] << 8 | d[off + 2] << 16 | (u32)d[off + 3] << 24;
}
void wr32(u8 *d, u32 off, u32 v) {
    d[off] = (u8)v; d[off + 1] = (u8)(v >> 8);
    d[off + 2] = (u8)(v >> 16); d[off + 3] = (u8)(v >> 24);
}

/* one s16 vertex component divided by 1 << shift, round half away from 0 */
u16 vtx_scale(u16 raw, int shift)
{
    int v = (short)raw;
    int half = 1 << (shift - 1);
    v = v >= 0 ? (v + half) >> shift : -((-v + half) >> shift);
    return (u16)v;
}

/* GX parameter-word counts; -1 = not a command. Geometry-bearing commands
   the rescale refuses are handled in the walk itself. */
signed char gx_params(u8 c)
{
    static const signed char t[0x42] = {
        0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1, 0, 1, 1, 1, 0, 16, 12, 16, 12, 9, 3, 3, -1, -1, -1,
        1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1,
        1, 1, 1, 1, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        1, 0,
    };
    return c < sizeof t ? t[c] : -1;
}

/* divide every vertex coordinate in a GX stream by 1 << shift, in place.
   1 on success; 0 refuses the stream (unknown command, or one whose
   geometry this rescale will not guess at: matrix loads, MTX_TRANS,
   VTX_10, VTX_DIFF -- neither source stream uses them). */
int stream_rescale(u8 *s, u32 len, int shift)
{
    u32 p = 0;
    while (p + 4 <= len) {
        u8 cmds[4];
        memcpy(cmds, s + p, 4);
        p += 4;
        for (int i = 0; i < 4; ++i) {
            u8 c = cmds[i];
            signed char n = gx_params(c);
            if (n < 0)
                return 0;
            if (c == 0x16 || c == 0x17 || c == 0x18 || c == 0x19 ||
                c == 0x1a || c == 0x1c || c == 0x24 || c == 0x28)
                return 0;
            if (p + (u32)n * 4 > len)
                return 0;
            if (c == 0x23) {
                u32 w0 = rd32(s, p), w1 = rd32(s, p + 4);
                w0 = vtx_scale((u16)w0, shift)
                     | ((u32)vtx_scale((u16)(w0 >> 16), shift) << 16);
                w1 = (w1 & 0xffff0000u) | vtx_scale((u16)w1, shift);
                wr32(s, p, w0);
                wr32(s, p + 4, w1);
            } else if (c == 0x25 || c == 0x26 || c == 0x27) {
                u32 w0 = rd32(s, p);
                w0 = vtx_scale((u16)w0, shift)
                     | ((u32)vtx_scale((u16)(w0 >> 16), shift) << 16);
                wr32(s, p, w0);
            }
            p += (u32)n * 4;
        }
    }
    return p == len;
}

struct WeaveDl { u32 map_n, map_src, stream_n, stream_src; const u8 *file; };

u8 *weave_build(u32 *out_len)
{
    u32 y_len = 0, m_len = 0;
    u8 *yd = catalog_file_load(g_yoshi_model, &y_len);
    u8 *md = catalog_file_load(g_mario_model, &m_len);
    u8 *out = 0;
    const char *why = "source file unreadable";
    struct WeaveDl dls[5];
    u32 y_scale, y_bo, y_texo, y_palo, y_mato, y_u2c;
    u32 m_bo, m_dlo, m_texo, m_palo, m_mato;
    u32 blob_lo, blob_hi;
    u32 w, dl_table, bone_off, blob_off;
    u32 tex_off, pal_off, mat_off;
    u32 sub_offs[5], tex_data[5], pal_data[5];
    int i;

    if (!yd || !md)
        goto refuse;
    why = "source header not the derived shape";
    if (y_len < 0x3c || m_len < 0x3c)
        goto refuse;
    y_scale = rd32(yd, 0x00);
    y_bo = rd32(yd, 0x08);
    y_texo = rd32(yd, 0x18);
    y_palo = rd32(yd, 0x20);
    y_mato = rd32(yd, 0x28);
    y_u2c = rd32(yd, 0x2c);
    m_bo = rd32(md, 0x08);
    m_dlo = rd32(md, 0x10);
    m_texo = rd32(md, 0x18);
    m_palo = rd32(md, 0x20);
    m_mato = rd32(md, 0x28);
    if (rd32(yd, 0x04) != 17 || rd32(md, 0x04) != 16 ||
        rd32(yd, 0x0c) != 5 || rd32(md, 0x0c) != 2 ||
        rd32(yd, 0x14) != 5 || rd32(yd, 0x1c) != 5 || rd32(yd, 0x24) != 5 ||
        rd32(md, 0x14) != 2 || rd32(md, 0x1c) != 2 || rd32(md, 0x24) != 2 ||
        y_scale == 0 || y_scale > 15)
        goto refuse;
    blob_lo = y_bo + 17 * 0x40;
    blob_hi = y_texo;
    if (blob_lo >= blob_hi || blob_hi > y_len ||
        y_u2c < blob_lo || y_u2c >= blob_hi)
        goto refuse;

    /* the five display lists: yoshi's wall (0..2), mario's body + eye */
    why = "display list table not the derived shape";
    for (i = 0; i < 5; ++i) {
        const u8 *d = i < 3 ? yd : md;
        u32 dlen = i < 3 ? y_len : m_len;
        u32 dlo = i < 3 ? rd32(yd, 0x10) : m_dlo;
        u32 e = dlo + (u32)(i < 3 ? i : i - 3) * 8;
        u32 sub;
        if (e + 8 > dlen || rd32(d, e) != 1)
            goto refuse;
        sub = rd32(d, e + 4);
        if (sub + 16 > dlen)
            goto refuse;
        dls[i].map_n = rd32(d, sub);
        dls[i].map_src = rd32(d, sub + 4);
        dls[i].stream_n = rd32(d, sub + 8);
        dls[i].stream_src = rd32(d, sub + 12);
        dls[i].file = d;
        if (dls[i].map_src + dls[i].map_n > dlen ||
            dls[i].stream_src + dls[i].stream_n > dlen)
            goto refuse;
    }

    /* one output buffer; the merge is subsets of both inputs plus padding */
    out = (u8 *)malloc((size_t)y_len + m_len + 4096);
    if (!out) {
        why = "out of host memory";
        goto refuse;
    }

    w = 0x3c;                              /* header written last */
    dl_table = w;
    memset(out + w, 0, 5 * 8);
    w += 5 * 8;
    for (i = 0; i < 5; ++i) {
        u32 sub = w, mp, sp;
        memset(out + w, 0, 16);
        w += 16;
        mp = w;
        memcpy(out + w, dls[i].file + dls[i].map_src, dls[i].map_n);
        if (i >= 3)                        /* the wall bone shifts mario's
                                              transform ids up by one */
            for (u32 k = 0; k < dls[i].map_n; ++k) {
                if (out[mp + k] >= 16) { why = "skin map out of range"; goto refuse; }
                out[mp + k] += 1;
            }
        w += dls[i].map_n;
        while (w % 4) out[w++] = 0;
        sp = w;
        memcpy(out + w, dls[i].file + dls[i].stream_src, dls[i].stream_n);
        if (i >= 3 && !stream_rescale(out + sp, dls[i].stream_n,
                                      (int)y_scale)) {
            why = "geometry stream refused the rescale";
            goto refuse;
        }
        w += dls[i].stream_n;
        while (w % 4) out[w++] = 0;
        wr32(out, sub, dls[i].map_n);
        wr32(out, sub + 4, mp);
        wr32(out, sub + 8, dls[i].stream_n);
        wr32(out, sub + 12, sp);
        sub_offs[i] = sub;
    }
    for (i = 0; i < 5; ++i) {
        wr32(out, dl_table + (u32)i * 8, 1);
        wr32(out, dl_table + (u32)i * 8 + 4, sub_offs[i]);
    }

    /* bones: yoshi's records and tree, mario's rest pose on the character */
    bone_off = w;
    memcpy(out + w, yd + y_bo, 17 * 0x40);
    w += 17 * 0x40;
    why = "bone names disagree between the sources";
    for (i = 2; i < 17; ++i) {
        u32 yr = bone_off + (u32)i * 0x40;
        u32 mr = m_bo + (u32)(i - 1) * 0x40;
        u32 yn = rd32(out, yr + 4), mn = rd32(md, mr + 4);
        if (yn >= y_len || mn >= m_len ||
            strcmp((const char *)yd + yn, (const char *)md + mn) != 0)
            goto refuse;
        memcpy(out + yr + 0x10, md + mr + 0x10, 0x20);
    }

    /* the blob (bone names, binding lists, the unk2c target), verbatim */
    blob_off = w;
    memcpy(out + w, yd + blob_lo, blob_hi - blob_lo);
    w += blob_hi - blob_lo;

    /* tex/pal/mat records and one fresh name pool for all fifteen */
    {
        u32 recs[3][5], names[3][5];
        u32 y_tab[3] = { y_texo, y_palo, y_mato };
        u32 m_tab[3] = { m_texo, m_palo, m_mato };
        u32 rsz[3] = { 0x14, 0x10, 0x30 };
        why = "record name outside its file";
        for (int t = 0; t < 3; ++t)
            for (i = 0; i < 5; ++i) {
                const u8 *d = i < 3 ? yd : md;
                u32 dlen = i < 3 ? y_len : m_len;
                u32 rec = (i < 3 ? y_tab[t] : m_tab[t])
                          + (u32)(i < 3 ? i : i - 3) * rsz[t];
                u32 nm;
                if (rec + rsz[t] > dlen)
                    goto refuse;
                recs[t][i] = rec;
                nm = rd32(d, rec);
                if (nm >= dlen)
                    goto refuse;
                names[t][i] = w;
                {
                    size_t sl = strlen((const char *)d + nm) + 1;
                    memcpy(out + w, d + nm, sl);
                    w += (u32)sl;
                }
            }
        while (w % 4) out[w++] = 0;

        tex_off = w;
        for (i = 0; i < 5; ++i) {
            memcpy(out + w, (i < 3 ? yd : md) + recs[0][i], 0x14);
            wr32(out, w, names[0][i]);
            w += 0x14;
        }
        pal_off = w;
        for (i = 0; i < 5; ++i) {
            memcpy(out + w, (i < 3 ? yd : md) + recs[1][i], 0x10);
            wr32(out, w, names[1][i]);
            w += 0x10;
        }
        mat_off = w;
        for (i = 0; i < 5; ++i) {
            memcpy(out + w, (i < 3 ? yd : md) + recs[2][i], 0x30);
            wr32(out, w, names[2][i]);
            if (i >= 3) {                 /* mario's tex/pal indices shift */
                wr32(out, w + 4, rd32(out, w + 4) + 3);
                wr32(out, w + 8, rd32(out, w + 8) + 3);
            }
            w += 0x30;
        }

        /* texture then palette data blobs, pointers patched as they land */
        why = "texture or palette data outside its file";
        for (int t = 0; t < 2; ++t)
            for (i = 0; i < 5; ++i) {
                const u8 *d = i < 3 ? yd : md;
                u32 dlen = i < 3 ? y_len : m_len;
                u32 rec = (t == 0 ? tex_off : pal_off)
                          + (u32)i * (t == 0 ? 0x14 : 0x10);
                u32 src = rd32(out, rec + 4), sz = rd32(out, rec + 8);
                if (src + sz > dlen)
                    goto refuse;
                (t == 0 ? tex_data : pal_data)[i] = w;
                memcpy(out + w, d + src, sz);
                w += sz;
                while (w % 4) out[w++] = 0;
                wr32(out, rec + 4, (t == 0 ? tex_data : pal_data)[i]);
            }
    }

    /* fixups: every yoshi pointer into the blob moves by one delta */
    {
        u32 delta = blob_off - blob_lo;
        why = "bone pointer outside the blob";
        for (i = 0; i < 17; ++i) {
            u32 r = bone_off + (u32)i * 0x40;
            static const u32 F[3] = { 4, 0x34, 0x38 };
            for (int k = 0; k < 3; ++k) {
                u32 v = rd32(out, r + F[k]);
                if (v < blob_lo || v >= blob_hi)
                    goto refuse;
                wr32(out, r + F[k], v + delta);
            }
        }
        /* header: yoshi's counts and scale, the new offsets, unk2c moved */
        memcpy(out, yd, 0x3c);
        wr32(out, 0x08, bone_off);
        wr32(out, 0x10, dl_table);
        wr32(out, 0x18, tex_off);
        wr32(out, 0x20, pal_off);
        wr32(out, 0x28, mat_off);
        wr32(out, 0x2c, y_u2c + delta);
    }

    free(yd);
    free(md);
    *out_len = w;
    return out;

refuse:
    fprintf(stderr, "[mods] LovesMeCharacter: weave refused (%s); the "
                    "direct swap serves without the wall\n", why);
    free(yd);
    free(md);
    free(out);
    return 0;
}

void woven_load(void)
{
    char path[520];
    FILE *f;
    if (g_woven_state)
        return;
    g_woven_state = -1;

    /* 1. the drop-in override file, when someone has made one */
    snprintf(path, sizeof path, "%s/build/assets/mods/lovesme_mario.bmd",
             mods_root());
    f = fopen(path, "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        g_woven_len = ftell(f);
        fseek(f, 0, SEEK_SET);
        g_woven = (u8 *)malloc(g_woven_len ? (size_t)g_woven_len : 1);
        if (g_woven && (long)fread(g_woven, 1, (size_t)g_woven_len, f)
                       == g_woven_len) {
            fclose(f);
            g_woven_state = 1;
            fprintf(stderr, "[mods] LovesMeCharacter mario: override model "
                            "%s (%ld bytes, wall kept, animations "
                            "untouched)\n", path, g_woven_len);
            return;
        }
        fclose(f);
        free(g_woven);
        g_woven = 0;
    }

    /* 2. the in-memory weave from the player's own extraction */
    {
        u32 len = 0;
        u8 *built = weave_build(&len);
        if (built) {
            g_woven = built;
            g_woven_len = (long)len;
            g_woven_state = 1;
            fprintf(stderr, "[mods] LovesMeCharacter mario: woven in memory "
                            "(%ld bytes, wall kept, animations untouched)\n",
                    g_woven_len);
            /* drift check against tools/lovesme_weave.py: dump the build so
               it can be compared byte for byte with the tool's output */
            {
                const char *dump = getenv("SM64DS_LOVESME_WEAVE_DUMP");
                if (dump && *dump) {
                    FILE *df = fopen(dump, "wb");
                    if (df) {
                        fwrite(g_woven, 1, (size_t)g_woven_len, df);
                        fclose(df);
                        fprintf(stderr, "[mods] weave dumped to %s\n", dump);
                    }
                }
            }
        }
    }
}

unsigned mod_map(unsigned fileID)
{
    if (!lovesme_on())
        return fileID;
    resolve_ids();
    if (g_yoshi_model && g_mario_model && fileID == g_yoshi_model) {
        woven_load();
        /* woven mode serves the yoshi id and swaps its BYTES in the filter,
           so mario_model_mg stays untouched for any other consumer */
        return g_woven_state > 0 ? fileID : g_mario_model;
    }
    return fileID;
}

/* Drop descriptor row `bone` from a decompressed .bca in place. Returns the
   new size, or `size` untouched when the buffer is not the shape the header
   of this file derives -- served unmodified and said once on stderr, because
   a cosmetic mod must degrade to a wrong-looking pose, never to a crash. */
u32 bca_drop_bone(u8 *d, u32 size, unsigned bone, const char *why)
{
    u32 nb, desc_off, rows_end;
    if (size < 0x18)
        goto refuse;
    nb = d[0] | d[1] << 8;
    desc_off = d[0x14] | d[0x15] << 8 | d[0x16] << 16 | (u32)d[0x17] << 24;
    rows_end = desc_off + nb * 36;
    if (nb <= bone || desc_off < 0x18 || rows_end != size)
        goto refuse;
    memmove(d + desc_off + bone * 36, d + desc_off + (bone + 1) * 36,
            (size_t)(nb - bone - 1) * 36);
    --nb;
    d[0] = (u8)nb;
    d[1] = (u8)(nb >> 8);
    return size - 36;
refuse:
    {
        static int said;
        if (!said++)
            fprintf(stderr, "[mods] %s: unexpected .bca shape (%u bytes); "
                            "serving it unmodified\n", why, size);
    }
    return size;
}

u32 mod_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!lovesme_on())
        return size;
    resolve_ids();
    if (!g_yoshi_model || !g_mario_model)
        return size;
    woven_load(); /* an anim can load before the model; decide the mode now */
    if (fileID == g_yoshi_model && g_woven_state > 0) {
        /* replace the master with a copy of the woven model. A copy, not the
           loaded image itself: the fs cache owns and may free what it holds */
        u8 *copy = (u8 *)malloc((size_t)g_woven_len);
        if (!copy)
            return size;
        memcpy(copy, g_woven, (size_t)g_woven_len);
        free(*data);
        *data = copy;
        return (u32)g_woven_len;
    }
    /* the anims are re-rigged ONLY for the 16-bone fallback; the woven
       model keeps all seventeen bones and wants them exactly as authored */
    if (g_woven_state > 0)
        return size;
    for (int i = 0; i < ANIM_COUNT; ++i)
        if (g_anims[i] && fileID == g_anims[i])
            return bca_drop_bone(*data, size, 1, "LovesMeCharacter");
    return size;
}

/* Installed before main by the CRT's initializer walk; every fs call the
   game makes happens far later. Targets that do not link this file keep the
   null hooks. */
struct InstallHooks {
    InstallHooks()
    {
        port_fs_mod_map = mod_map;
        port_fs_mod_filter = mod_filter;
    }
} g_install;

} /* namespace */
