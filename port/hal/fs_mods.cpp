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
// ---- THE FIRST MOD: the Loves Me...? character swap ------------------------
//
// dScMgFlower_c (hal/scene_mg_flower.cpp, scene 390) stars the minigame
// archive's Yoshi: MG/yoshi_model.bmd and the four animations
// MG/yoshi_{wait,pick,like,hate}.bca, all loaded by FAT file id through
// SharedFilePtr. LovesMeCharacter picks who sits on the wall instead.
//
// yoshi_model.bmd is not just Yoshi. Bone 1, "polySurface689", is the CASTLE
// WALL the character sits on during the rules card -- three tiled materials
// (mc_til_str/mc_tile00/tri_tile, palettes f_game1..3) baked into the
// character file because the ROM only ever stars Yoshi here. So a character
// swap is never a plain file swap: build_character() below composes a fresh
// model IN MEMORY from the player's own extraction -- yoshi's wall, skeleton
// and animations kept, the chosen character's geometry and textures grafted
// in. tools/lovesme_compose.py is the REFERENCE implementation: this file is
// its line-for-line C++ twin, kept BYTE-IDENTICAL (verified with
// SM64DS_LOVESME_WEAVE_DUMP=<path> against the tool's output), and the
// tool's docstring carries the whole file-format derivation. The short form:
//
//   * every playable character shares the 16-bone player rig; yoshi's MG
//     file is that rig plus the wall bone at index 1, and its 17-track
//     animations serve UNTOUCHED over every composed model;
//   * header +0x2c points at the TRANSFORM SLOT TABLE (u16 slot -> bone).
//     DL skin-map bytes are SLOT numbers in a PER-FILE namespace, so maps
//     translate src slot -> src bone -> merged bone -> merged slot;
//   * main-game character textures are 4x4-compressed (format 5), whose
//     palette-index companion rides at data+size (Model::LoadTexAndPal
//     passes t->data + sz). This scene's VRAM cursors never budget the
//     compressed format -- the ROM loads none here -- so ANY compressed
//     upload stomps the scene's regular texture VRAM. Every format-5
//     texture is transcoded to plain 256color at compose time, including
//     table entries no material references, because LoadTexAndPal uploads
//     those too;
//   * regular Luigi and Wario are BODY + HEAD pairs (the head is a separate
//     one-bone model bound to face_1); arc0[126]'s mat_head names a
//     runtime-swapped texture variant that decodes to noise, so it is
//     retargeted to luigi_head_1, the real cap texture;
//   * the three colored Yoshis are the MG model with its body palette
//     re-authored through the stacked VS color rows in arc0[196]
//     (green/red/blue/yellow, 32 bytes each).
//
// FALLBACK: a source file that is not the shape the derivation pinned
// refuses loudly and serves mario_model_mg.bmd directly for the model id
// (the wall vanishes -- known cosmetic gap), with the four animations
// re-rigged by dropping the wall bone's descriptor row so the 16-bone model
// gets the 16 rows that are his. A colored Yoshi that cannot be recolored
// falls back to plain Yoshi instead: the model the ROM serves is already
// right, only the color is missing.
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
#ifdef _WIN32
#include <windows.h>
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" int host_setting_lovesme_character(void);   /* hal/host_settings.cpp */
extern "C" int host_setting_custom_palette(void);      /* hal/host_settings.cpp */
extern "C" const char *port_fs_catalog_path(unsigned); /* hal/fs.cpp */
extern "C" unsigned port_fs_interior_id(const char *); /* hal/fs.cpp */

extern "C" {
extern unsigned (*port_fs_mod_map)(unsigned fileID);
extern u32 (*port_fs_mod_filter)(unsigned fileID, u8 **data, u32 size);
}

namespace {

enum { CATALOG_SCAN_MAX = 4096, ANIM_COUNT = 4 };

/* 1-based values of host_setting_lovesme_character, matching
   host_settings.cpp's LOVESME_KEY table. Metal Wario existed briefly and
   was cut on looks: without the env-map material path he renders as a gold
   statue, and the owner ruled him out. */
enum { CH_OFF = 0, CH_MARIO = 1, CH_YOSHI_RED, CH_YOSHI_BLUE,
       CH_YOSHI_YELLOW, CH_LUIGI, CH_WARIO };
const char *const CH_NAME[7] = { "off", "mario", "yoshi_red", "yoshi_blue",
                                 "yoshi_yellow", "luigi", "wario" };

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
    if (!g_yoshi_model || !g_mario_model)
        fprintf(stderr, "[mods] LovesMeCharacter: MG models not in the "
                        "catalog; the swap is off for this run\n");
}

/* The boot latch: -1 until first asked, then the setting as it was then. */
int lovesme_character(void)
{
    static int v = -1;
    if (v < 0)
        v = host_setting_lovesme_character();
    return v;
}

/* The composed model bytes, built once on first ask. woven_state: 0 not yet
   tried, 1 have bytes, -1 no bytes. g_fallback_mario says what a -1 means:
   1 = serve mario directly with re-rigged animations (compose characters),
   0 = serve plain yoshi (a colored yoshi whose recolor refused). */
u8 *g_woven;
long g_woven_len;
int g_woven_state;
int g_fallback_mario;

#ifndef PORT_REPO_ROOT
#define PORT_REPO_ROOT "."
#endif

const char *mods_root(void)
{
    const char *env = getenv("SM64DS_ASSET_ROOT");
    return env && *env ? env : PORT_REPO_ROOT;
}

extern "C" void DecompressLZ16(void *src, void *dst); /* hal/fs.cpp */

/* raw bytes -> decompressed malloc'd buffer (LZ77-sniffed, like the fs cache
   fill). Consumes nothing; the caller frees its raw buffer. */
u8 *decompress_maybe(u8 *raw, long fsize, u32 *len_out)
{
    if (fsize > 8 && memcmp(raw, "LZ77", 4) == 0) {
        u32 dec = (raw[4] | raw[5] << 8 | raw[6] << 16 | (u32)raw[7] << 24)
                  >> 8;
        u8 *out = (u8 *)malloc(dec ? dec : 1);
        if (out)
            DecompressLZ16(raw + 4, out);
        *len_out = dec;
        return out;
    }
    {
        u8 *out = (u8 *)malloc(fsize ? (size_t)fsize : 1);
        if (out)
            memcpy(out, raw, (size_t)fsize);
        *len_out = (u32)fsize;
        return out;
    }
}

u8 *file_load(const char *path, u32 *len_out)
{
    FILE *f = fopen(path, "rb");
    long fsize;
    u8 *raw, *out;
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
    out = decompress_maybe(raw, fsize, len_out);
    free(raw);
    return out;
}

/* A catalog file's decompressed bytes, malloc'd, or 0. Kept separate from
   the fs cache so the composer can read its SOURCE files without touching
   the cache it is about to feed. */
u8 *catalog_file_load(unsigned fileID, u32 *len_out)
{
    char path[520];
    const char *rel = port_fs_catalog_path(fileID);
    if (!rel)
        return 0;
    snprintf(path, sizeof path, "%s/extracted/dsd/files/%s", mods_root(), rel);
    return file_load(path, len_out);
}

/* One member of ARCHIVE/arc0.narc, decompressed. NARC: sections from 0x10,
   BTAF holds the member count and start/end pairs, GMIF holds the data. */
u8 *arc0_member(unsigned idx, u32 *len_out)
{
    char path[520];
    u32 nlen = 0;
    u8 *n;
    u32 off, fat_off = 0, nfiles = 0, img = 0, s = 0, e = 0;
    u8 *out = 0;
    snprintf(path, sizeof path, "%s/extracted/dsd/files/ARCHIVE/arc0.narc",
             mods_root());
    n = file_load(path, &nlen);
    if (!n)
        return 0;
    if (nlen < 0x10 || memcmp(n, "NARC", 4) != 0)
        goto done;
    off = 0x10;
    while (off + 8 <= nlen) {
        u32 size = n[off + 4] | n[off + 5] << 8 | n[off + 6] << 16
                   | (u32)n[off + 7] << 24;
        if (memcmp(n + off, "BTAF", 4) == 0) {
            nfiles = n[off + 8] | n[off + 9] << 8 | n[off + 10] << 16
                     | (u32)n[off + 11] << 24;
            fat_off = off + 12;
        } else if (memcmp(n + off, "GMIF", 4) == 0) {
            img = off + 8;
        }
        if (!size)
            break;
        off += size;
    }
    if (!img || idx >= nfiles || fat_off + idx * 8 + 8 > nlen)
        goto done;
    s = n[fat_off + idx * 8] | n[fat_off + idx * 8 + 1] << 8
        | n[fat_off + idx * 8 + 2] << 16 | (u32)n[fat_off + idx * 8 + 3] << 24;
    e = n[fat_off + idx * 8 + 4] | n[fat_off + idx * 8 + 5] << 8
        | n[fat_off + idx * 8 + 6] << 16 | (u32)n[fat_off + idx * 8 + 7] << 24;
    if (img + e > nlen || s >= e)
        goto done;
    out = decompress_maybe(n + img + s, (long)(e - s), len_out);
done:
    free(n);
    return out;
}

/* ---- little helpers shared by the composer -------------------------------*/

u32 rd32(const u8 *d, u32 off) {
    return d[off] | d[off + 1] << 8 | d[off + 2] << 16 | (u32)d[off + 3] << 24;
}
u16 rd16(const u8 *d, u32 off) { return (u16)(d[off] | d[off + 1] << 8); }
void wr32(u8 *d, u32 off, u32 v) {
    d[off] = (u8)v; d[off + 1] = (u8)(v >> 8);
    d[off + 2] = (u8)(v >> 16); d[off + 3] = (u8)(v >> 24);
}
void wr16(u8 *d, u32 off, u16 v) {
    d[off] = (u8)v; d[off + 1] = (u8)(v >> 8);
}

/* one s16 vertex component divided by 1 << shift, round half away from 0 */
u16 vtx_scale(u16 raw, int shift)
{
    int v = (short)raw;
    int half = 1 << (shift - 1);
    v = v >= 0 ? (v + half) >> shift : -((-v + half) >> shift);
    return (u16)v;
}

/* GX parameter-word counts; -1 = not a command. */
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
   1 on success; 0 refuses (unknown command, or one whose geometry this
   rescale will not guess at: matrix loads, MTX_TRANS, VTX_10, VTX_DIFF --
   no source stream uses them). shift 0 still walks, changing nothing, so a
   malformed stream refuses at any scale. */
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
            if (shift > 0 && c == 0x23) {
                u32 w0 = rd32(s, p), w1 = rd32(s, p + 4);
                w0 = vtx_scale((u16)w0, shift)
                     | ((u32)vtx_scale((u16)(w0 >> 16), shift) << 16);
                w1 = (w1 & 0xffff0000u) | vtx_scale((u16)w1, shift);
                wr32(s, p, w0);
                wr32(s, p + 4, w1);
            } else if (shift > 0 &&
                       (c == 0x25 || c == 0x26 || c == 0x27)) {
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

/* ---- parsed model, sized to the real inventory with room to spare --------*/

enum { MAX_BONES = 20, MAX_DLS = 18, MAX_ENTS = 12, MAX_BIND = 6,
       MAX_SLOTS = 24 };

struct Ent {
    u8 rec[0x30];       /* the record, 0x14/0x10/0x30 of it meaningful */
    const char *name;   /* into the source buffer, or a literal */
    u8 *data;           /* owned malloc when owns_data, else into source */
    u32 blob;           /* bytes at data (incl. compressed companion) */
    u32 size_field;     /* the record's size word */
    int owns_data;
    int transcoded;
    int pair_pal;       /* positional palette index in the SOURCE, or -1 */
};

struct Dl {
    u8 *map;            /* owned */
    u32 map_n;
    u8 *stream;         /* owned */
    u32 stream_n;
};

struct Bone {
    u8 rec[0x40];
    const char *name;
    u8 mats[MAX_BIND], dls[MAX_BIND];
    u32 nbind;
};

struct Bmd {
    u32 scale, nb, ndl, ntex, npal, nmat, nslots;
    struct Bone bones[MAX_BONES];
    struct Dl dls[MAX_DLS];
    struct Ent texs[MAX_ENTS], pals[MAX_ENTS], mats[MAX_ENTS];
    u16 slots[MAX_SLOTS];
    u8 *raw;            /* owned source buffer the pointers reach into */
    u32 raw_len;
};

void bmd_free(struct Bmd *m)
{
    if (!m)
        return;
    for (u32 i = 0; i < m->ndl; ++i) {
        free(m->dls[i].map);
        free(m->dls[i].stream);
    }
    for (u32 i = 0; i < m->ntex; ++i)
        if (m->texs[i].owns_data)
            free(m->texs[i].data);
    for (u32 i = 0; i < m->npal; ++i)
        if (m->pals[i].owns_data)
            free(m->pals[i].data);
    free(m->raw);
    free(m);
}

/* the C++ twin of lovesme_compose.py's parse_bmd; 0 on any shape surprise */
struct Bmd *bmd_parse(u8 *raw, u32 len)
{
    struct Bmd *m;
    u32 bo, dlo, texo, palo, mato, slot_off, max_slot = 0;
    if (!raw || len < 0x3c) {
        free(raw);
        return 0;
    }
    m = (struct Bmd *)calloc(1, sizeof *m);
    if (!m) {
        free(raw);
        return 0;
    }
    m->raw = raw;
    m->raw_len = len;
    m->scale = rd32(raw, 0x00);
    m->nb = rd32(raw, 0x04);
    bo = rd32(raw, 0x08);
    m->ndl = rd32(raw, 0x0c);
    dlo = rd32(raw, 0x10);
    m->ntex = rd32(raw, 0x14);
    texo = rd32(raw, 0x18);
    m->npal = rd32(raw, 0x1c);
    palo = rd32(raw, 0x20);
    m->nmat = rd32(raw, 0x24);
    mato = rd32(raw, 0x28);
    slot_off = rd32(raw, 0x2c);
    if (m->nb == 0 || m->nb > MAX_BONES || m->ndl > MAX_DLS ||
        m->ntex > MAX_ENTS || m->npal > MAX_ENTS || m->nmat > MAX_ENTS ||
        bo + m->nb * 0x40 > len)
        goto bad;
    for (u32 i = 0; i < m->nb; ++i) {
        struct Bone *b = &m->bones[i];
        u32 nm, cnt, mi, di;
        memcpy(b->rec, raw + bo + i * 0x40, 0x40);
        nm = rd32(b->rec, 4);
        cnt = rd32(b->rec, 0x30);
        mi = rd32(b->rec, 0x34);
        di = rd32(b->rec, 0x38);
        if (nm >= len || cnt > MAX_BIND || mi + cnt > len || di + cnt > len)
            goto bad;
        b->name = (const char *)raw + nm;
        b->nbind = cnt;
        memcpy(b->mats, raw + mi, cnt);
        memcpy(b->dls, raw + di, cnt);
    }
    for (u32 i = 0; i < m->ndl; ++i) {
        u32 e = dlo + i * 8, sub, mc, mp, ss, sp;
        if (e + 8 > len || rd32(raw, e) != 1)
            goto bad;
        sub = rd32(raw, e + 4);
        if (sub + 16 > len)
            goto bad;
        mc = rd32(raw, sub);
        mp = rd32(raw, sub + 4);
        ss = rd32(raw, sub + 8);
        sp = rd32(raw, sub + 12);
        if (mp + mc > len || sp + ss > len)
            goto bad;
        m->dls[i].map = (u8 *)malloc(mc ? mc : 1);
        m->dls[i].stream = (u8 *)malloc(ss ? ss : 1);
        if (!m->dls[i].map || !m->dls[i].stream)
            goto bad;
        memcpy(m->dls[i].map, raw + mp, mc);
        memcpy(m->dls[i].stream, raw + sp, ss);
        m->dls[i].map_n = mc;
        m->dls[i].stream_n = ss;
        for (u32 k = 0; k < mc; ++k)
            if (m->dls[i].map[k] > max_slot)
                max_slot = m->dls[i].map[k];
    }
    for (int t = 0; t < 3; ++t) {
        struct Ent *arr = t == 0 ? m->texs : t == 1 ? m->pals : m->mats;
        u32 n = t == 0 ? m->ntex : t == 1 ? m->npal : m->nmat;
        u32 off = t == 0 ? texo : t == 1 ? palo : mato;
        u32 rsz = t == 0 ? 0x14 : t == 1 ? 0x10 : 0x30;
        for (u32 i = 0; i < n; ++i) {
            struct Ent *e = &arr[i];
            u32 nm;
            if (off + (i + 1) * rsz > len)
                goto bad;
            memcpy(e->rec, raw + off + i * rsz, rsz);
            nm = rd32(e->rec, 0);
            if (nm >= len)
                goto bad;
            e->name = (const char *)raw + nm;
            e->pair_pal = -1;
            if (t != 2) {
                u32 dp = rd32(e->rec, 4), sz = rd32(e->rec, 8);
                e->blob = sz;
                if (t == 0) {
                    u32 flags = rd32(e->rec, 0x10);
                    if (((flags >> 26) & 7) == 5)
                        e->blob = sz + sz / 2;
                }
                if (dp + e->blob > len)
                    goto bad;
                e->data = raw + dp;
                e->size_field = sz;
            }
        }
    }
    if (m->ndl && slot_off + (max_slot + 1) * 2 <= len) {
        m->nslots = max_slot + 1;
        if (m->nslots > MAX_SLOTS)
            goto bad;
        for (u32 s = 0; s < m->nslots; ++s) {
            m->slots[s] = rd16(raw, slot_off + s * 2);
            if (m->slots[s] >= m->nb)
                goto bad;
        }
    } else if (m->ndl) {
        goto bad;
    }
    return m;
bad:
    bmd_free(m);
    return 0;
}

struct Bmd *bmd_load_catalog(unsigned fileID)
{
    u32 len = 0;
    u8 *raw = catalog_file_load(fileID, &len);
    return bmd_parse(raw, len);
}

struct Bmd *bmd_load_arc0(unsigned idx)
{
    u32 len = 0;
    u8 *raw = arc0_member(idx, &len);
    return bmd_parse(raw, len);
}

/* ---- 4x4-compressed -> plain 256color, the twin of the tool's version ----*/

int transcode_compressed(struct Ent *tex, struct Ent *pal)
{
    u32 flags = rd32(tex->rec, 0x10);
    u32 w = 8u << ((flags >> 20) & 7);
    u32 h = 8u << ((flags >> 23) & 7);
    const u8 *main_ = tex->data;
    const u8 *comp = tex->data + tex->size_field;
    const u8 *pdata = pal->data;
    u32 plen = pal->size_field < pal->blob ? pal->size_field : pal->blob;
    int *texels;             /* -1 = transparent */
    u16 palette[257];
    u32 npal = 0;
    int transparent = 0;
    u8 *out;
    u8 *new_pal;
    u32 blocks_w = w / 4, nblocks = tex->size_field / 4;

    if (((flags >> 26) & 7) != 5 || w * h == 0 || w * h > 128 * 128)
        return 0;
    texels = (int *)malloc(w * h * sizeof(int));
    out = (u8 *)malloc(w * h);
    if (!texels || !out) {
        free(texels);
        free(out);
        return 0;
    }

    for (u32 bi = 0; bi < nblocks; ++bi) {
        u32 block = rd32(main_, bi * 4);
        u16 info = rd16(comp, bi * 2);
        u32 base = (u32)(info & 0x3fff) * 4;
        int mode = info >> 14;
        int cols[4];
        u16 c0 = base + 2 <= plen ? rd16(pdata, base) : 0;
        u16 c1 = base + 4 <= plen ? rd16(pdata, base + 2) : 0;
#define MIXC(a, b, na, nb, div) \
        (u16)(((((a) & 31) * (na) + ((b) & 31) * (nb)) / (div)) \
              | (((((a) >> 5 & 31) * (na) + ((b) >> 5 & 31) * (nb)) / (div)) << 5) \
              | (((((a) >> 10 & 31) * (na) + ((b) >> 10 & 31) * (nb)) / (div)) << 10))
        cols[0] = c0;
        cols[1] = c1;
        if (mode == 0) {
            cols[2] = base + 6 <= plen ? rd16(pdata, base + 4) : 0;
            cols[3] = -1;
        } else if (mode == 1) {
            cols[2] = MIXC(c0, c1, 1, 1, 2);
            cols[3] = -1;
        } else if (mode == 2) {
            cols[2] = base + 6 <= plen ? rd16(pdata, base + 4) : 0;
            cols[3] = base + 8 <= plen ? rd16(pdata, base + 6) : 0;
        } else {
            cols[2] = MIXC(c0, c1, 5, 3, 8);
            cols[3] = MIXC(c0, c1, 3, 5, 8);
        }
#undef MIXC
        {
            u32 bx = (bi % blocks_w) * 4, by = (bi / blocks_w) * 4;
            for (int t = 0; t < 16; ++t) {
                u32 x = bx + (u32)(t % 4), y = by + (u32)(t / 4);
                if (x < w && y < h)
                    texels[y * w + x] = cols[(block >> (t * 2)) & 3];
            }
        }
    }

    for (u32 i = 0; i < w * h; ++i)
        if (texels[i] < 0)
            transparent = 1;
    if (transparent)
        palette[npal++] = 0;
    for (u32 i = 0; i < w * h; ++i) {
        int v = texels[i];
        u32 idx = 0;
        if (v < 0) {
            out[i] = 0;
            continue;
        }
        {
            u32 first = transparent ? 1u : 0u;
            int found = 0;
            for (u32 j = first; j < npal; ++j)
                if (palette[j] == (u16)v) {
                    idx = j;
                    found = 1;
                    break;
                }
            if (!found) {
                if (npal >= 256) {
                    u32 best = first, bd = 0xffffffffu;
                    for (u32 j = first; j < npal; ++j) {
                        int dr = (v & 31) - (palette[j] & 31);
                        int dg = ((v >> 5) & 31) - ((palette[j] >> 5) & 31);
                        int db = ((v >> 10) & 31) - ((palette[j] >> 10) & 31);
                        u32 d = (u32)(dr * dr + dg * dg + db * db);
                        if (d < bd) {
                            best = j;
                            bd = d;
                        }
                    }
                    idx = best;
                } else {
                    idx = npal;
                    palette[npal++] = (u16)v;
                }
            }
        }
        out[i] = (u8)idx;
    }

    {
        u32 pbytes = npal * 2;
        while (pbytes % 16)
            pbytes += 2;
        new_pal = (u8 *)calloc(pbytes ? pbytes : 1, 1);
        if (!new_pal) {
            free(texels);
            free(out);
            return 0;
        }
        for (u32 j = 0; j < npal; ++j)
            wr16(new_pal, j * 2, palette[j]);
        if (pal->owns_data)
            free(pal->data);
        pal->data = new_pal;
        pal->blob = pal->size_field = pbytes;
        pal->owns_data = 1;
    }
    if (tex->owns_data)
        free(tex->data);
    tex->data = out;
    tex->blob = tex->size_field = w * h;
    tex->owns_data = 1;
    flags = (flags & ~((7u << 26) | 0x20000000u)) | (4u << 26);
    if (transparent)
        flags |= 0x20000000u;
    wr32(tex->rec, 0x10, flags);
    tex->transcoded = 1;
    free(texels);
    return 1;
}

/* ---- the composer: wall + body (+ optional 1-bone head on face_1) --------*/

struct Out {
    u8 *p;
    u32 n, cap;
    int oom;
};

void out_bytes(struct Out *o, const void *src, u32 len)
{
    if (o->oom)
        return;
    if (o->n + len > o->cap) {
        u32 ncap = (o->n + len) * 2 + 4096;
        u8 *np = (u8 *)realloc(o->p, ncap);
        if (!np) {
            o->oom = 1;
            return;
        }
        o->p = np;
        o->cap = ncap;
    }
    if (src)
        memcpy(o->p + o->n, src, len);
    else
        memset(o->p + o->n, 0, len);
    o->n += len;
}

void out_align(struct Out *o, u32 a)
{
    static const u8 z[4] = { 0, 0, 0, 0 };
    while (o->n % a && !o->oom)
        out_bytes(o, z, 1);
}

const char *const PLAYER_BONES[15] = {
    "skl_root", "leg_l1", "leg_l2", "ankle_l1", "leg_r1", "leg_r2",
    "ankle_r1", "spin", "arm_l1", "arm_l2", "wrist_l1", "arm_r1", "arm_r2",
    "wrist_r1", "face_1",
};

u8 *compose(struct Bmd *yoshi, struct Bmd *body, struct Bmd *head,
            u32 *out_len, const char **why)
{
    struct Dl dls[MAX_DLS];
    struct Ent texs[MAX_ENTS * 2], pals[MAX_ENTS * 2], mats[MAX_ENTS * 2];
    u32 ndl = 0, ntex = 0, npal = 0, nmat = 0;
    struct Bone bones[17];
    u32 body_dl0 = 0, body_mat0 = 0, head_dl0 = 0, head_mat0 = 0;
    struct Out o = { 0, 0, 0, 0 };
    u32 dl_table, bone_off, dummy, unk2c;
    u32 sub_offs[MAX_DLS];
    u32 name_offs[17];
    u32 binds_mi[17], binds_di[17];
    u32 pool_off[MAX_ENTS * 6];
    const char *pool_name[MAX_ENTS * 6];
    u32 npool = 0;
    u32 tex_off, pal_off, mat_off;
    u8 *result = 0;
    u32 i;

    memset(dls, 0, sizeof dls);
    *why = "source not the derived shape";
    if (!yoshi || !body || yoshi->scale != 4 || yoshi->nb != 17 ||
        yoshi->ndl < 3 || yoshi->ntex < 3 || yoshi->npal < 3 ||
        yoshi->nmat < 3 || body->nb != 16 || (head && head->nb != 1))
        goto refuse;
    for (i = 0; i < 15; ++i)
        if (strcmp(body->bones[i + 1].name, PLAYER_BONES[i]) != 0)
            goto refuse;
    if (3 + body->ndl + (head ? head->ndl : 0) > MAX_DLS ||
        3 + body->ntex + (head ? head->ntex : 0) > MAX_ENTS * 2 ||
        3 + body->npal + (head ? head->npal : 0) > MAX_ENTS * 2 ||
        3 + body->nmat + (head ? head->nmat : 0) > MAX_ENTS * 2)
        goto refuse;

    /* gather: wall = yoshi 0..2 in every table, then body, then head. DLs
       are copied (maps rewritten, streams rescaled); records are copied. */
    for (i = 0; i < 3; ++i) {
        struct Dl *d = &dls[ndl];
        d->map = (u8 *)malloc(yoshi->dls[i].map_n ? yoshi->dls[i].map_n : 1);
        d->stream = (u8 *)malloc(yoshi->dls[i].stream_n
                                 ? yoshi->dls[i].stream_n : 1);
        if (!d->map || !d->stream)
            goto refuse;
        memcpy(d->map, yoshi->dls[i].map, yoshi->dls[i].map_n);
        memcpy(d->stream, yoshi->dls[i].stream, yoshi->dls[i].stream_n);
        d->map_n = yoshi->dls[i].map_n;
        d->stream_n = yoshi->dls[i].stream_n;
        ndl++;
        texs[ntex++] = yoshi->texs[i];
        pals[npal++] = yoshi->pals[i];
        mats[nmat++] = yoshi->mats[i];
    }
    for (int piece_i = 0; piece_i < (head ? 2 : 1); ++piece_i) {
        struct Bmd *piece = piece_i == 0 ? body : head;
        int shift = 4 - (int)piece->scale;
        u32 tex_base = ntex, pal_base = npal;
        if (piece_i == 0) {
            body_dl0 = ndl;
            body_mat0 = nmat;
        } else {
            head_dl0 = ndl;
            head_mat0 = nmat;
        }
        if (shift < 0) {
            *why = "source stored larger than the wall file";
            goto refuse;
        }
        for (i = 0; i < piece->ndl; ++i) {
            struct Dl *d = &dls[ndl];
            u32 mn = piece_i == 1 ? 1 : piece->dls[i].map_n;
            d->map = (u8 *)malloc(mn ? mn : 1);
            d->stream = (u8 *)malloc(piece->dls[i].stream_n
                                     ? piece->dls[i].stream_n : 1);
            if (!d->map || !d->stream)
                goto refuse;
            if (piece_i == 1) {
                d->map[0] = 15;         /* everything rides face_1 (16) */
            } else {
                /* merged map byte = merged bone - 1 = the source bone the
                   piece's own slot table names (see the tool's comment) */
                for (u32 k = 0; k < piece->dls[i].map_n; ++k) {
                    u8 slot = piece->dls[i].map[k];
                    if (slot >= piece->nslots)
                        goto refuse;
                    d->map[k] = (u8)piece->slots[slot];
                }
            }
            d->map_n = mn;
            memcpy(d->stream, piece->dls[i].stream, piece->dls[i].stream_n);
            d->stream_n = piece->dls[i].stream_n;
            if (!stream_rescale(d->stream, d->stream_n, shift)) {
                *why = "geometry stream refused the rescale";
                goto refuse;
            }
            ndl++;
        }
        for (i = 0; i < piece->ntex; ++i) {
            texs[ntex] = piece->texs[i];
            if (piece->ntex == piece->npal)
                texs[ntex].pair_pal = (int)(pal_base + i);
            ntex++;
        }
        for (i = 0; i < piece->npal; ++i)
            pals[npal++] = piece->pals[i];
        for (i = 0; i < piece->nmat; ++i) {
            struct Ent *e = &mats[nmat++];
            *e = piece->mats[i];
            wr32(e->rec, 4, rd32(e->rec, 4) + tex_base);
            wr32(e->rec, 8, rd32(e->rec, 8) + pal_base);
        }
    }

    /* every compressed texture becomes plain 256color; LoadTexAndPal
       uploads unreferenced entries too, so ALL of them transcode. Pairing:
       material first, name+"_pl" second, positional in the source third. */
    {
        int tex_pal[MAX_ENTS * 2];
        for (i = 0; i < ntex; ++i)
            tex_pal[i] = -1;
        for (i = 3; i < nmat; ++i) {
            u32 t = rd32(mats[i].rec, 4), p = rd32(mats[i].rec, 8);
            if (t < ntex && p < npal)
                tex_pal[t] = (int)p;
        }
        for (i = 0; i < ntex; ++i) {
            u32 flags = rd32(texs[i].rec, 0x10);
            struct Ent *pal = 0;
            if (((flags >> 26) & 7) != 5 || texs[i].transcoded)
                continue;
            if (tex_pal[i] >= 0) {
                pal = &pals[tex_pal[i]];
            } else {
                char want[64];
                snprintf(want, sizeof want, "%s_pl", texs[i].name);
                for (u32 j = 0; j < npal; ++j)
                    if (strcmp(pals[j].name, want) == 0) {
                        pal = &pals[j];
                        break;
                    }
                if (!pal && texs[i].pair_pal >= 0)
                    pal = &pals[texs[i].pair_pal];
            }
            if (!pal) {
                *why = "no palette pairing for a compressed texture";
                goto refuse;
            }
            if (!transcode_compressed(&texs[i], pal)) {
                *why = "compressed texture refused the transcode";
                goto refuse;
            }
        }
    }

    /* the merged 17 bones: yoshi's records and tree, the body's rest pose
       and render flags on the character bones, bindings re-authored */
    for (i = 0; i < 17; ++i) {
        struct Bone *b = &bones[i];
        memcpy(b->rec, yoshi->bones[i].rec, 0x40);
        b->name = yoshi->bones[i].name;
        b->nbind = 0;
        if (i == 1) {
            b->nbind = yoshi->bones[1].nbind;
            memcpy(b->mats, yoshi->bones[1].mats, b->nbind);
            memcpy(b->dls, yoshi->bones[1].dls, b->nbind);
        } else {
            struct Bone *src = &body->bones[i == 0 ? 0 : i - 1];
            memcpy(b->rec + 0x10, src->rec + 0x10, 0x20);
            memcpy(b->rec + 0x3c, src->rec + 0x3c, 4);
            if (src->nbind > MAX_BIND)
                goto refuse;
            b->nbind = src->nbind;
            for (u32 k = 0; k < src->nbind; ++k) {
                b->mats[k] = (u8)(src->mats[k] + body_mat0);
                b->dls[k] = (u8)(src->dls[k] + body_dl0);
            }
        }
    }
    memcpy(bones[0].rec + 0x10, yoshi->bones[0].rec + 0x10, 0x20);
    if (head) {
        struct Bone *hr = &head->bones[0];
        struct Bone *f = &bones[16];
        if (f->nbind + hr->nbind > MAX_BIND)
            goto refuse;
        for (u32 k = 0; k < hr->nbind; ++k) {
            f->mats[f->nbind] = (u8)(hr->mats[k] + head_mat0);
            f->dls[f->nbind] = (u8)(hr->dls[k] + head_dl0);
            f->nbind++;
        }
    }

    /* ---- emit, in exactly the reference tool's order ---------------------*/
    out_bytes(&o, 0, 0x3c);
    dl_table = o.n;
    out_bytes(&o, 0, ndl * 8);
    for (i = 0; i < ndl; ++i) {
        u32 sub = o.n, mp, sp;
        out_bytes(&o, 0, 16);
        mp = o.n;
        out_bytes(&o, dls[i].map, dls[i].map_n);
        out_align(&o, 4);
        sp = o.n;
        out_bytes(&o, dls[i].stream, dls[i].stream_n);
        out_align(&o, 4);
        if (o.oom)
            goto oom;
        wr32(o.p, sub, dls[i].map_n);
        wr32(o.p, sub + 4, mp);
        wr32(o.p, sub + 8, dls[i].stream_n);
        wr32(o.p, sub + 12, sp);
        sub_offs[i] = sub;
    }
    for (i = 0; i < ndl; ++i) {
        wr32(o.p, dl_table + i * 8, 1);
        wr32(o.p, dl_table + i * 8 + 4, sub_offs[i]);
    }

    bone_off = o.n;
    out_bytes(&o, 0, 17 * 0x40);

    for (i = 0; i < 17; ++i) {
        u32 j, off = 0;
        int seen = 0;
        for (j = 0; j < i; ++j)
            if (strcmp(bones[j].name, bones[i].name) == 0) {
                off = name_offs[j];
                seen = 1;
                break;
            }
        if (!seen) {
            off = o.n;
            out_bytes(&o, bones[i].name, (u32)strlen(bones[i].name) + 1);
        }
        name_offs[i] = off;
    }
    for (i = 0; i < 17; ++i) {
        if (bones[i].nbind) {
            binds_mi[i] = o.n;
            out_bytes(&o, bones[i].mats, bones[i].nbind);
            binds_di[i] = o.n;
            out_bytes(&o, bones[i].dls, bones[i].nbind);
        } else {
            binds_mi[i] = binds_di[i] = 0;   /* the dummy, patched below */
        }
    }
    dummy = o.n;
    out_bytes(&o, 0, 1);
    out_align(&o, 2);
    unk2c = o.n;                             /* the merged slot table */
    for (i = 0; i < 16; ++i) {
        u8 s2[2] = { (u8)(i + 1), 0 };
        out_bytes(&o, s2, 2);
    }
    out_align(&o, 4);
    if (o.oom)
        goto oom;

    for (i = 0; i < 17; ++i) {
        u8 *rec = o.p + bone_off + i * 0x40;
        memcpy(rec, bones[i].rec, 0x40);
        wr32(rec, 0, i);
        wr32(rec, 4, name_offs[i]);
        wr32(rec, 0x30, bones[i].nbind);
        wr32(rec, 0x34, bones[i].nbind ? binds_mi[i] : dummy);
        wr32(rec, 0x38, bones[i].nbind ? binds_di[i] : dummy);
    }

    for (int t = 0; t < 3; ++t) {
        struct Ent *arr = t == 0 ? texs : t == 1 ? pals : mats;
        u32 n = t == 0 ? ntex : t == 1 ? npal : nmat;
        for (i = 0; i < n; ++i) {
            u32 j, off = 0;
            int seen = 0;
            for (j = 0; j < npool; ++j)
                if (strcmp(pool_name[j], arr[i].name) == 0) {
                    off = pool_off[j];
                    seen = 1;
                    break;
                }
            if (!seen) {
                off = o.n;
                out_bytes(&o, arr[i].name, (u32)strlen(arr[i].name) + 1);
                pool_name[npool] = arr[i].name;
                pool_off[npool++] = off;
            }
            wr32(arr[i].rec, 0, off);
        }
    }
    out_align(&o, 4);

    tex_off = o.n;
    for (i = 0; i < ntex; ++i)
        out_bytes(&o, texs[i].rec, 0x14);
    pal_off = o.n;
    for (i = 0; i < npal; ++i)
        out_bytes(&o, pals[i].rec, 0x10);
    mat_off = o.n;
    for (i = 0; i < nmat; ++i)
        out_bytes(&o, mats[i].rec, 0x30);
    for (int t = 0; t < 2; ++t) {
        struct Ent *arr = t == 0 ? texs : pals;
        u32 n = t == 0 ? ntex : npal;
        u32 roff = t == 0 ? tex_off : pal_off;
        u32 rsz = t == 0 ? 0x14 : 0x10;
        for (i = 0; i < n; ++i) {
            u32 dpos = o.n;
            out_bytes(&o, arr[i].data, arr[i].blob);
            out_align(&o, 4);
            if (o.oom)
                goto oom;
            wr32(o.p, roff + i * rsz + 4, dpos);
            wr32(o.p, roff + i * rsz + 8, arr[i].size_field);
        }
    }
    if (o.oom)
        goto oom;

    wr32(o.p, 0x00, 4);
    wr32(o.p, 0x04, 17);
    wr32(o.p, 0x08, bone_off);
    wr32(o.p, 0x0c, ndl);
    wr32(o.p, 0x10, dl_table);
    wr32(o.p, 0x14, ntex);
    wr32(o.p, 0x18, tex_off);
    wr32(o.p, 0x1c, npal);
    wr32(o.p, 0x20, pal_off);
    wr32(o.p, 0x24, nmat);
    wr32(o.p, 0x28, mat_off);
    wr32(o.p, 0x2c, unk2c);
    wr32(o.p, 0x30, 0);
    wr32(o.p, 0x34, 0);
    wr32(o.p, 0x38, 0);

    *out_len = o.n;
    result = o.p;
    o.p = 0;
    goto done;

oom:
    *why = "out of host memory";
refuse:
done:
    for (i = 0; i < ndl; ++i) {
        free(dls[i].map);
        free(dls[i].stream);
    }
    /* transcodes ran on the LOCAL copies, so their buffers are ours alone */
    for (i = 0; i < ntex; ++i)
        if (texs[i].owns_data)
            free(texs[i].data);
    for (i = 0; i < npal; ++i)
        if (pals[i].owns_data)
            free(pals[i].data);
    free(o.p);
    return result;
}

/* ---- the colored Yoshis: the MG palette re-authored through the VS rows --*/

int palette_named(const u8 *bmd, u32 len, const char *name, u32 *data_out,
                  u32 *size_out)
{
    u32 npal = rd32(bmd, 0x1c), palo = rd32(bmd, 0x20);
    for (u32 i = 0; i < npal; ++i) {
        u32 rec = palo + i * 0x10;
        u32 nm;
        if (rec + 0x10 > len)
            return 0;
        nm = rd32(bmd, rec);
        if (nm < len && strcmp((const char *)bmd + nm, name) == 0) {
            *data_out = rd32(bmd, rec + 4);
            *size_out = rd32(bmd, rec + 8);
            return *data_out + *size_out <= len;
        }
    }
    return 0;
}

u8 *build_recolor(int target_row, u32 *out_len, const char **why)
{
    u32 mg_len = 0, arc_len = 0, mg_pal = 0, mg_psz = 0, vs_pal = 0,
        vs_psz = 0;
    u8 *mg = catalog_file_load(g_yoshi_model, &mg_len);
    u8 *arc = arc0_member(196, &arc_len);
    u16 rows[4][16];

    *why = "recolor sources unreadable";
    if (!mg || !arc)
        goto refuse;
    *why = "recolor palettes not the derived shape";
    if (!palette_named(mg, mg_len, "yoshi_all_pl", &mg_pal, &mg_psz) ||
        mg_psz != 32 ||
        !palette_named(arc, arc_len, "yoshi_all_16p_pl", &vs_pal, &vs_psz) ||
        vs_psz != 128)
        goto refuse;
    for (int r = 0; r < 4; ++r)
        for (int i = 0; i < 16; ++i)
            rows[r][i] = rd16(arc, vs_pal + (u32)r * 32 + (u32)i * 2);

    for (int i = 0; i < 16; ++i) {
        u16 c = rd16(mg, mg_pal + (u32)i * 2);
        int cr = c & 31, cg = (c >> 5) & 31, cb = (c >> 10) & 31;
        int best = 0;
        u32 bd = 0xffffffffu;
        for (int j = 0; j < 16; ++j) {
            u16 g = rows[0][j];
            int dr = cr - (g & 31), dg = cg - ((g >> 5) & 31),
                db = cb - ((g >> 10) & 31);
            u32 d = (u32)(dr * dr + dg * dg + db * db);
            if (d < bd) {
                best = j;
                bd = d;
            }
        }
        if (rows[0][best] == rows[target_row][best])
            continue;
        {
            u16 g = rows[0][best], t = rows[target_row][best];
            int nr = (t & 31) + cr - (g & 31);
            int ng = ((t >> 5) & 31) + cg - ((g >> 5) & 31);
            int nb = ((t >> 10) & 31) + cb - ((g >> 10) & 31);
            nr = nr < 0 ? 0 : nr > 31 ? 31 : nr;
            ng = ng < 0 ? 0 : ng > 31 ? 31 : ng;
            nb = nb < 0 ? 0 : nb > 31 ? 31 : nb;
            wr16(mg, mg_pal + (u32)i * 2,
                 (u16)(nr | (ng << 5) | (nb << 10)));
        }
    }
    free(arc);
    *out_len = mg_len;
    return mg;

refuse:
    free(mg);
    free(arc);
    return 0;
}

/* ---- character dispatch --------------------------------------------------*/

u8 *build_character(int ch, u32 *out_len, const char **why)
{
    struct Bmd *yoshi = 0, *body = 0, *head = 0;
    u8 *out = 0;

    *why = "unknown character";
    if (ch == CH_YOSHI_RED || ch == CH_YOSHI_BLUE || ch == CH_YOSHI_YELLOW)
        return build_recolor(ch - CH_YOSHI_RED + 1, out_len, why);

    *why = "source files unreadable";
    yoshi = bmd_load_catalog(g_yoshi_model);
    if (!yoshi)
        goto done;
    switch (ch) {
    case CH_MARIO:
        body = bmd_load_catalog(g_mario_model);
        break;
    case CH_LUIGI:
        body = bmd_load_arc0(128);
        head = bmd_load_arc0(126);
        /* arc0[126]'s mat_head names luigi_head_2, a runtime-swapped
           variant that decodes to noise; luigi_head_1 is the cap */
        if (head)
            for (u32 i = 0; i < head->nmat; ++i)
                if (strcmp(head->mats[i].name, "mat_head") == 0) {
                    wr32(head->mats[i].rec, 4, 5);
                    wr32(head->mats[i].rec, 8, 5);
                }
        break;
    case CH_WARIO:
        body = bmd_load_arc0(186);
        head = bmd_load_arc0(184);
        break;
    default:
        goto done;
    }
    if (!body || ((ch == CH_LUIGI || ch == CH_WARIO) && !head))
        goto done;
    out = compose(yoshi, body, head, out_len, why);

done:
    bmd_free(yoshi);
    bmd_free(body);
    bmd_free(head);
    return out;
}

void woven_load(void)
{
    const int ch = lovesme_character();
    if (g_woven_state)
        return;
    g_woven_state = -1;
    g_fallback_mario = !(ch >= CH_YOSHI_RED && ch <= CH_YOSHI_YELLOW);

    /* DEV override: SM64DS_LOVESME_MODEL=<path> serves that file's bytes for
       the model id, animations untouched. An ENV VAR, deliberately not a
       build/assets path: a "build/assets/<name>" literal in the exe is a
       promise kit_smoke holds the extractor to (port/kit_assets.txt), and an
       optional file the extractor never writes would break that contract. An
       env path names nothing the kit owes anyone, and a launcher-driven run
       never sets it. This is how candidate builds are iterated without a
       rebuild per try. */
    {
        const char *ov = getenv("SM64DS_LOVESME_MODEL");
        if (ov && *ov) {
            u32 len = 0;
            g_woven = file_load(ov, &len);
            if (g_woven) {
                g_woven_len = (long)len;
                g_woven_state = 1;
                fprintf(stderr, "[mods] LovesMeCharacter: override model "
                                "%s (%ld bytes)\n", ov, g_woven_len);
                return;
            }
            fprintf(stderr, "[mods] LovesMeCharacter: override %s "
                            "unreadable; composing instead\n", ov);
        }
    }

    /* the in-memory build from the player's own extraction */
    {
        u32 len = 0;
        const char *why = "?";
        u8 *built = build_character(ch, &len, &why);
        if (built) {
            g_woven = built;
            g_woven_len = (long)len;
            g_woven_state = 1;
            fprintf(stderr, "[mods] LovesMeCharacter %s: composed in memory "
                            "(%ld bytes, wall kept, animations untouched)\n",
                    CH_NAME[ch >= 0 && ch <= 6 ? ch : 0], g_woven_len);
            /* drift check against tools/lovesme_compose.py: dump the build
               so it can be compared byte for byte with the tool's output */
            {
                const char *dump = getenv("SM64DS_LOVESME_WEAVE_DUMP");
                if (dump && *dump) {
                    FILE *df = fopen(dump, "wb");
                    if (df) {
                        fwrite(g_woven, 1, (size_t)g_woven_len, df);
                        fclose(df);
                        fprintf(stderr, "[mods] build dumped to %s\n", dump);
                    }
                }
            }
        } else {
            fprintf(stderr, "[mods] LovesMeCharacter %s: compose refused "
                            "(%s); %s\n",
                    CH_NAME[ch >= 0 && ch <= 6 ? ch : 0], why,
                    g_fallback_mario
                        ? "the direct mario swap serves without the wall"
                        : "Yoshi serves as the ROM does");
        }
    }
}

/* ---- THE SECOND MOD: CustomPalette ----------------------------------------
   Recolors whatever the combo file says to recolor -- in practice the four
   playable characters -- by writing replacement color sets into BMD palette
   blobs as their files come through the filter. The code knows NOTHING about
   who is recolored: every target arrives as a (file id, palette index, color
   set) record in palettes/combo<N>.pal, found beside the exe the way the gap
   art is, so new combos and new targets need a new file, never a new build.

   The combo file is COMPILED, not authored: a human (or the Studio) writes
   colors in a readable spec and palmod compiles it to this blob, so the
   parser here stays a dozen dumb lines. Format, little-endian throughout:

       "PALM"  u16 version=2  u16 record count, then per record:
       u8 pathLen  u8 nameLen  u16 color count
       char path[pathLen]    the target FILE, by name: a NARC interior
                             path ("data/player/mario_model.bmd") or the
                             tail of a catalog path for a loose file.
                             Named, never numbered, because files.tsv and
                             the mount bases are generated from the
                             player's own dump -- the same reason
                             resolve_ids() above works by path. Resolved
                             once, when the combo loads.
       char name[nameLen]    the palette inside that BMD, by its own name
                             in the palette table
       u16 colors[count]     RGB555 as the DS stores palettes; count must
                             equal the palette's, whole sets only

   WHY PALETTE ENTRIES ONLY. Character textures are format 5, 4x4-compressed,
   whose index data maps texels to palette slots -- but the COLORS all live in
   the palette blob, so replacing color words recolors the character without
   touching a single index byte, the file's size, or any offset in it. The
   patch is in place on the decompressed master: nothing moves, nothing is
   reallocated, and a combo file that is absent, malformed, or disagrees with
   the BMD it names degrades to the ROM's colors with one line on stderr,
   never to a crash. Same rules as every mod: off is the ROM, boot-latched,
   the game's own code never learns. */

int palette_combo(void)
{
    static int v = -1;
    if (v < 0)
        v = host_setting_custom_palette();
    return v;
}

struct PalPatch {
    unsigned file_id;   /* resolved at load; 0 = not in this dump, disabled */
    u16 count;
    char path[128];     /* the spec's target file path */
    char name[64];      /* the spec's palette name */
    const u8 *colors;   /* count*2 bytes into g_pal_blob, LE, any alignment */
    int said;           /* each record reports once, patched or refused */
};

u8 *g_pal_blob;
struct PalPatch *g_pal_patch;
unsigned g_pal_npatch;
int g_pal_state;        /* 0 not tried, 1 loaded, -1 absent or refused */

/* The gap art's probe order: beside the exe (the bundle), then the asset
   root (the repo or SM64DS_ASSET_ROOT), then the working directory. */
FILE *palette_open(int combo, char *shown, size_t cap)
{
    char path[1024];
#ifdef _WIN32
    {
        char exe[MAX_PATH];
        DWORD n = GetModuleFileNameA(NULL, exe, MAX_PATH);
        if (n > 0 && n < MAX_PATH) {
            char *slash = strrchr(exe, '\\');
            char *fwd = strrchr(exe, '/');
            if (fwd && (!slash || fwd > slash))
                slash = fwd;
            if (slash) {
                *slash = '\0';
                snprintf(path, sizeof path, "%s\\palettes\\combo%d.pal",
                         exe, combo);
                FILE *f = fopen(path, "rb");
                if (f) {
                    snprintf(shown, cap, "%s", path);
                    return f;
                }
            }
        }
    }
#endif
    snprintf(path, sizeof path, "%s/palettes/combo%d.pal", mods_root(),
             combo);
    {
        FILE *f = fopen(path, "rb");
        if (f) {
            snprintf(shown, cap, "%s", path);
            return f;
        }
    }
    snprintf(path, sizeof path, "palettes/combo%d.pal", combo);
    {
        FILE *f = fopen(path, "rb");
        if (f) {
            snprintf(shown, cap, "%s", path);
            return f;
        }
    }
    return 0;
}

void palette_release(void)
{
    free(g_pal_blob);
    free(g_pal_patch);
    g_pal_blob = 0;
    g_pal_patch = 0;
    g_pal_npatch = 0;
}

void palette_load(void)
{
    const int combo = palette_combo();
    char shown[1024];
    FILE *f;
    long len;
    u32 nrec, cur;
    if (g_pal_state)
        return;
    g_pal_state = -1;
    f = palette_open(combo, shown, sizeof shown);
    if (!f) {
        fprintf(stderr, "[mods] CustomPalette %d: no palettes/combo%d.pal "
                        "beside the exe, the asset root, or the working "
                        "directory; colors stay the ROM's\n", combo, combo);
        return;
    }
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 8 || len > 4 * 1024 * 1024)
        goto refuse_close;
    g_pal_blob = (u8 *)malloc((size_t)len);
    if (!g_pal_blob || (long)fread(g_pal_blob, 1, len, f) != len)
        goto refuse_close;
    fclose(f);
    f = 0;
    if (memcmp(g_pal_blob, "PALM", 4) != 0 || rd16(g_pal_blob, 4) != 2)
        goto refuse;
    nrec = rd16(g_pal_blob, 6);
    if (nrec == 0 || nrec > 512)
        goto refuse;
    g_pal_patch = (struct PalPatch *)calloc(nrec, sizeof *g_pal_patch);
    if (!g_pal_patch)
        goto refuse;
    cur = 8;
    for (u32 r = 0; r < nrec; ++r) {
        struct PalPatch *p = &g_pal_patch[r];
        u32 pl, nl;
        if (cur + 4 > (u32)len)
            goto refuse;
        pl = g_pal_blob[cur];
        nl = g_pal_blob[cur + 1];
        p->count = rd16(g_pal_blob, cur + 2);
        if (pl == 0 || pl >= sizeof p->path || nl == 0 ||
            nl >= sizeof p->name || p->count == 0 || p->count > 4096 ||
            cur + 4 + pl + nl + (u32)p->count * 2 > (u32)len)
            goto refuse;
        memcpy(p->path, g_pal_blob + cur + 4, pl);
        memcpy(p->name, g_pal_blob + cur + 4 + pl, nl);
        p->colors = g_pal_blob + cur + 4 + pl + nl;
        cur += 4 + pl + nl + (u32)p->count * 2;
        ++g_pal_npatch;
    }
    /* resolve every record's file NAME to this dump's id: the loose catalog
       first (the resolve_ids scan), then the archives' own name tables */
    for (u32 r = 0; r < g_pal_npatch; ++r) {
        struct PalPatch *p = &g_pal_patch[r];
        for (unsigned id = 0; id < CATALOG_SCAN_MAX && !p->file_id; ++id) {
            const char *cp = port_fs_catalog_path(id);
            if (cp && path_ends_with(cp, p->path))
                p->file_id = id;
        }
        if (!p->file_id)
            p->file_id = port_fs_interior_id(p->path);
        if (!p->file_id)
            fprintf(stderr, "[mods] CustomPalette %d: %s is not in this "
                            "dump; that record is off for this run\n",
                    combo, p->path);
    }
    g_pal_state = 1;
    fprintf(stderr, "[mods] CustomPalette %d: %u palette patch record(s) "
                    "(%s)\n", combo, g_pal_npatch, shown);
    return;
refuse_close:
    fclose(f);
refuse:
    palette_release();
    fprintf(stderr, "[mods] CustomPalette %d: %s is not a palmod blob this "
                    "build understands; colors stay the ROM's\n",
            combo, shown);
}

/* Exact NUL-terminated string at offset `at` in a bounded buffer. */
int str_at_is(const u8 *d, u32 size, u32 at, const char *want)
{
    u32 j = 0;
    for (; want[j]; ++j)
        if (at + j >= size || (char)d[at + j] != want[j])
            return 0;
    return at + j < size && d[at + j] == '\0';
}

/* One record against one served file: 0 on success, else the refusal. The
   palette is found by ITS name in the file's own table, so the record and
   the file agree on identity or nothing is written. */
const char *palette_try(const struct PalPatch *p, u8 *d, u32 size)
{
    u32 npal, palo, dp, sz;
    const u8 *rec = 0;
    if (size < 0x3c)
        return "file too small for a BMD header";
    npal = rd32(d, 0x1c);
    palo = rd32(d, 0x20);
    if (npal > 64)
        return "palette count is not a BMD's";
    if (palo > size || npal * 0x10 > size - palo)
        return "palette table past end of file";
    for (u32 i = 0; i < npal; ++i) {
        const u8 *r = d + palo + i * 0x10;
        u32 nm = rd32(r, 0);
        if (nm < size && str_at_is(d, size, nm, p->name)) {
            rec = r;
            break;
        }
    }
    if (!rec)
        return "no palette of that name in the file";
    dp = rd32(rec, 4);
    sz = rd32(rec, 8);
    if (sz != (u32)p->count * 2)
        return "color count disagrees with the BMD";
    if (dp > size || sz > size - dp)
        return "palette data past end of file";
    memcpy(d + dp, p->colors, sz);
    return 0;
}

u32 palette_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!palette_combo())
        return size;
    palette_load();
    if (g_pal_state <= 0)
        return size;
    for (unsigned i = 0; i < g_pal_npatch; ++i) {
        struct PalPatch *p = &g_pal_patch[i];
        const char *why;
        if (!p->file_id || p->file_id != fileID)
            continue;
        why = palette_try(p, *data, size);
        if (p->said++)
            continue;
        if (why)
            fprintf(stderr, "[mods] CustomPalette %d: %s '%s' REFUSED: %s; "
                            "served as the ROM has it\n",
                    palette_combo(), p->path, p->name, why);
        else
            fprintf(stderr, "[mods] CustomPalette %d: %s '%s' recolored, "
                            "%u colors\n",
                    palette_combo(), p->path, p->name, p->count);
    }
    return size;
}

unsigned mod_map(unsigned fileID)
{
    if (!lovesme_character())
        return fileID;
    resolve_ids();
    if (g_yoshi_model && g_mario_model && fileID == g_yoshi_model) {
        woven_load();
        /* composed mode serves the yoshi id and swaps its BYTES in the
           filter, so mario_model_mg stays untouched for other consumers */
        if (g_woven_state > 0)
            return fileID;
        return g_fallback_mario ? g_mario_model : fileID;
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

u32 lovesme_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!lovesme_character())
        return size;
    resolve_ids();
    if (!g_yoshi_model || !g_mario_model)
        return size;
    woven_load(); /* an anim can load before the model; decide the mode now */
    if (fileID == g_yoshi_model && g_woven_state > 0) {
        /* replace the master with a copy of the composed model. A copy, not
           the built image itself: the fs cache owns and may free its data */
        u8 *copy = (u8 *)malloc((size_t)g_woven_len);
        if (!copy)
            return size;
        memcpy(copy, g_woven, (size_t)g_woven_len);
        free(*data);
        *data = copy;
        return (u32)g_woven_len;
    }
    /* the anims are re-rigged ONLY for the 16-bone mario fallback; every
       composed model keeps all seventeen bones and wants them as authored */
    if (g_woven_state > 0 || !g_fallback_mario)
        return size;
    for (int i = 0; i < ANIM_COUNT; ++i)
        if (g_anims[i] && fileID == g_anims[i])
            return bca_drop_bone(*data, size, 1, "LovesMeCharacter");
    return size;
}

/* The installed filter is a chain, each mod deciding for itself whether the
   file is its business. Loves Me first because it can REPLACE the buffer;
   the palette patch then edits whatever bytes are actually being served. */
u32 mod_filter(unsigned fileID, u8 **data, u32 size)
{
    size = lovesme_filter(fileID, data, size);
    size = palette_filter(fileID, data, size);
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
