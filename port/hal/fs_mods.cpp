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
#include "vs_width.h"   /* run vs16: the port's player width */
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "vs_palette_gen.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" int host_setting_lovesme_character(void);   /* hal/host_settings.cpp */
extern "C" int host_setting_custom_palette(void);      /* hal/host_settings.cpp */
extern "C" const char *host_setting_character_palette(int); /* host_settings */
extern "C" int host_setting_character_palette_any(void);    /* host_settings */
extern "C" int host_setting_yoshi_builtin_row(void);        /* host_settings */
extern "C" const char *port_fs_catalog_path(unsigned); /* hal/fs.cpp */
extern "C" unsigned port_fs_interior_id(const char *); /* hal/fs.cpp */
/* romdata.py's host-shaped copy of the ROM's mount table: which runtime id
   range each NARC owns. hal/fs.cpp declares it the same way and
   build/port/host-src/romdata.c defines it. */
struct port_arc_entry { unsigned short base, end; const char *narc; };
extern "C" struct port_arc_entry port_archive_map[13]; /* host-src/romdata.c */

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

/* One member of an archive, decompressed. NARC: sections from 0x10, BTAF
   holds the member count and start/end pairs, GMIF holds the data. `rel` is
   the archive's path under extracted/dsd/files, the same spelling
   port_archive_map carries. */
u8 *narc_member(const char *rel, unsigned idx, u32 *len_out)
{
    char path[520];
    u32 nlen = 0;
    u8 *n;
    u32 off, fat_off = 0, nfiles = 0, img = 0, s = 0, e = 0;
    u8 *out = 0;
    snprintf(path, sizeof path, "%s/extracted/dsd/files/%s", mods_root(), rel);
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

u8 *arc0_member(unsigned idx, u32 *len_out)
{
    return narc_member("ARCHIVE/arc0.narc", idx, len_out);
}

/* Resolve a file NAME to this dump's runtime id: the loose catalog first,
   then the archives' own BTNF name tables. The ONE resolver every mod in
   this file uses, so a target named in a combo file and a target named in
   the table below can never disagree about what a path means. 0 when no
   file in this dump answers to that name. */
unsigned resolve_file_by_name(const char *path)
{
    for (unsigned id = 0; id < CATALOG_SCAN_MAX; ++id) {
        const char *cp = port_fs_catalog_path(id);
        if (cp && path_ends_with(cp, path))
            return id;
    }
    return port_fs_interior_id(path);
}

/* Decompressed bytes of any file the game can serve, by NAME. Loose files
   come out of the catalog; an archive interior is sliced out of whichever
   archive's id range owns the resolved id. Used to read the ROM's OWN data
   back out of the player's extraction -- see the Yoshi rows below -- so it
   deliberately reads the file rather than waiting for one to come past the
   filter: nothing promises the order files load in. */
u8 *file_by_name(const char *path, u32 *len_out)
{
    unsigned id;
    for (unsigned i = 0; i < CATALOG_SCAN_MAX; ++i) {
        const char *cp = port_fs_catalog_path(i);
        if (cp && path_ends_with(cp, path))
            return catalog_file_load(i, len_out);
    }
    id = port_fs_interior_id(path);
    if (!id)
        return 0;
    for (int i = 0; i < 13; ++i) {
        struct port_arc_entry *e = &port_archive_map[i];
        if (id >= e->base && id < e->end)
            return narc_member(e->narc, (unsigned)(id - e->base), len_out);
    }
    return 0;
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

/* THE PER-CHARACTER PALETTE PICKER below; declared here because this key
   stands down when that one is set, and the two blocks read in the other
   order. */
int pc_any(void);

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
   root (the repo or SM64DS_ASSET_ROOT), then the working directory. `base`
   is the file's basename without the extension, so the combo key and the
   per-character keys look in exactly the same three places. */
FILE *palette_open_named(const char *base, char *shown, size_t cap)
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
                snprintf(path, sizeof path, "%s\\palettes\\%s.pal", exe, base);
                FILE *f = fopen(path, "rb");
                if (f) {
                    snprintf(shown, cap, "%s", path);
                    return f;
                }
            }
        }
    }
#endif
    snprintf(path, sizeof path, "%s/palettes/%s.pal", mods_root(), base);
    {
        FILE *f = fopen(path, "rb");
        if (f) {
            snprintf(shown, cap, "%s", path);
            return f;
        }
    }
    snprintf(path, sizeof path, "palettes/%s.pal", base);
    {
        FILE *f = fopen(path, "rb");
        if (f) {
            snprintf(shown, cap, "%s", path);
            return f;
        }
    }
    return 0;
}

FILE *palette_open(int combo, char *shown, size_t cap)
{
    char base[32];
    snprintf(base, sizeof base, "combo%d", combo);
    return palette_open_named(base, shown, cap);
}

void palette_release(void)
{
    free(g_pal_blob);
    free(g_pal_patch);
    g_pal_blob = 0;
    g_pal_patch = 0;
    g_pal_npatch = 0;
}

/* Read an open .pal into a fresh blob plus a fresh record array, both the
   caller's to free. Returns the record count, or 0 for anything that is not
   a palmod v2 blob this build understands -- the caller says so in its own
   words, because "combo 2" and "PaletteLuigi" are different sentences to a
   player. Each record's colors point INTO the blob, so the two live and die
   together. Closes the file either way.

   ONE PARSER for both keys, deliberately: the combo file and the four
   per-character files are the same format compiled by the same tool, and a
   second copy of these bounds checks would be a second place for them to
   drift. */
unsigned palm_read(FILE *f, u8 **blob_out, struct PalPatch **patch_out)
{
    u8 *blob = 0;
    struct PalPatch *patch = 0;
    long len;
    u32 nrec, cur;
    unsigned got = 0;
    *blob_out = 0;
    *patch_out = 0;
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (len < 8 || len > 4 * 1024 * 1024)
        goto refuse;
    blob = (u8 *)malloc((size_t)len);
    if (!blob || (long)fread(blob, 1, len, f) != len)
        goto refuse;
    fclose(f);
    f = 0;
    if (memcmp(blob, "PALM", 4) != 0 || rd16(blob, 4) != 2)
        goto refuse;
    nrec = rd16(blob, 6);
    if (nrec == 0 || nrec > 512)
        goto refuse;
    patch = (struct PalPatch *)calloc(nrec, sizeof *patch);
    if (!patch)
        goto refuse;
    cur = 8;
    for (u32 r = 0; r < nrec; ++r) {
        struct PalPatch *p = &patch[r];
        u32 pl, nl;
        if (cur + 4 > (u32)len)
            goto refuse;
        pl = blob[cur];
        nl = blob[cur + 1];
        p->count = rd16(blob, cur + 2);
        if (pl == 0 || pl >= sizeof p->path || nl == 0 ||
            nl >= sizeof p->name || p->count == 0 || p->count > 4096 ||
            cur + 4 + pl + nl + (u32)p->count * 2 > (u32)len)
            goto refuse;
        memcpy(p->path, blob + cur + 4, pl);
        memcpy(p->name, blob + cur + 4 + pl, nl);
        p->colors = blob + cur + 4 + pl + nl;
        cur += 4 + pl + nl + (u32)p->count * 2;
        ++got;
    }
    *blob_out = blob;
    *patch_out = patch;
    return got;
refuse:
    if (f)
        fclose(f);
    free(blob);
    free(patch);
    return 0;
}

void palette_load(void)
{
    const int combo = palette_combo();
    char shown[1024];
    FILE *f;
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
    g_pal_npatch = palm_read(f, &g_pal_blob, &g_pal_patch);
    if (!g_pal_npatch) {
        palette_release();
        fprintf(stderr, "[mods] CustomPalette %d: %s is not a palmod blob "
                        "this build understands; colors stay the ROM's\n",
                combo, shown);
        return;
    }
    /* resolve every record's file NAME to this dump's id: the loose catalog
       first (the resolve_ids scan), then the archives' own name tables */
    for (u32 r = 0; r < g_pal_npatch; ++r) {
        struct PalPatch *p = &g_pal_patch[r];
        p->file_id = resolve_file_by_name(p->path);
        if (!p->file_id)
            fprintf(stderr, "[mods] CustomPalette %d: %s is not in this "
                            "dump; that record is off for this run\n",
                    combo, p->path);
    }
    g_pal_state = 1;
    fprintf(stderr, "[mods] CustomPalette %d: %u palette patch record(s) "
                    "(%s)\n", combo, g_pal_npatch, shown);
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
    /* The four per-character keys replace this one entirely; pc_load says so
       once, on stderr, so a support log never has to guess which mod wrote a
       color. See THE PER-CHARACTER PALETTE PICKER below. */
    if (pc_any())
        return size;
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

/* ---- THE THIRD MOD: the per-character palette picker ----------------------
   PaletteMario, PaletteLuigi, PaletteWario and PaletteYoshi: one key per
   playable character, each naming the colors that character wears. "" or an
   absent key is the ROM, and the ROM is the default.

   WHY FOUR KEYS AND NOT ONE MORE COMBO. CustomPalette above picks a whole
   FILE and applies every record in it, so a player who likes one author's
   Mario and another author's Luigi cannot have both. These keys are keyed on
   the CHARACTER instead: each names a palette file of its own, and only the
   records aimed at that character's own files are applied out of it. One
   authored file can therefore serve all four keys with four different
   answers, and the same file can be Mario's under one key and ignored under
   another.

   WHICH FILES ARE "THAT CHARACTER'S" is the table PC_FILES below and nothing
   else: body, head with cap, head without cap, plus for Luigi the two
   texture-swap files that drive his body and capped head. Ownership is
   decided on the RESOLVED FILE ID, never on the spelling of a path, because
   a suffix test cannot tell data/player/mario_head_cap.bmd from
   data/player/b_mario_head_cap.bmd and a mod that quietly recolored balloon
   Mario when asked for Mario would be a bug nobody could see. A record whose
   file resolves to somebody else's is dropped in silence at load; a record
   whose file is not in the dump AT ALL is said out loud, because that one is
   always either a typo or a dump problem.

   APPLIED RECORDS KEEP EVERY RULE CustomPalette HAS. palette_try is the same
   function: the palette is found by ITS OWN name in the served file's table,
   the color count must equal the palette's exactly, nothing moves and
   nothing is reallocated, and every disagreement refuses out loud and serves
   the ROM's colors.

   ---- THE BUILT-IN YOSHI COLORS -------------------------------------------
   PaletteYoshi also takes "yoshi:green", "yoshi:red", "yoshi:blue" and
   "yoshi:yellow", which need no file: they are the ROM's own four-player
   colors, read out of the player's own extraction.

   THE ROM'S SHAPE, verified against the extraction rather than assumed:
   data/player/yoshi_model.bmd holds ONE palette, yoshi_all_16p_pl, and it is
   128 bytes -- four stacked 16-color rows, and their colors say plainly
   which is which (row 0 greens, row 1 reds, row 2 blues, row 3 yellows).
   Both head files, yoshi_head.bmd and yoshi_head_fill.bmd, hold a palette of
   the SAME NAME that is 32 bytes, and those 32 bytes are byte-identical to
   the body's row 0.

   HOW VS PICKS A ROW, from the matched source and not from guesswork:
   func_ov002_020e5948 (src/func_ov002_020e5948.c) ends its resource load
   with Player+0x61C = the Yoshi BODY model's material[0] palette base plus
   (playerNo << 1). Player::Render (src/_ZN6Player6RenderEv.cpp) then writes
   that one value into +0x20 of every runtime material record -- but only
   when data_0209f2d8 == 1, which is VS, and param1 == 3, which is Yoshi.

   AND THAT IS THE ANSWER TO THE HEAD QUESTION. Render does it TWICE, in two
   separate loops: once over the BODY model's materials, and once over the
   HEAD model's, with the same value both times. So in VS the head files are
   not edited, not resized and not reloaded -- their own 16-color palette is
   simply never sampled, because every head material has been re-pointed at
   the BODY's palette, shifted to the chosen row. Both head materials in both
   head files already share palette 0 (yoshi_all_tx and yoshi_eye_tx alike),
   so the shift moves the skin and the eyes together, as one set.

   The arrangement proves itself: player 0 shifts by nothing, so VS green
   samples the body's row 0 -- and the head files' own palette IS the body's
   row 0, byte for byte. Adventure Yoshi and VS-green Yoshi are therefore the
   same picture, which is only true if one step of playerNo is exactly one
   16-color row.

   SO THE BUILT-IN IS: put the chosen row where row 0 is in the body file,
   and put the SAME row into each head file's 16-color palette. Rows 1..3 of
   the body are left exactly as authored -- nothing in adventure points at
   them -- and the head palettes stay 16 colors, because they are 16-color
   palettes for 16-color textures and growing one to 64 would upload three
   rows of colors nothing samples over the top of whatever VRAM follows.

   THE EYES COME ALONG, AND THAT IS CORRECT HERE. Recoloring Yoshi's skin
   moves his eye colors too, because they share the palette. For these four
   built-ins that is not a risk but the point: the colors being copied are
   the ROM's own authored rows, and the ROM moves only SEVEN of the sixteen
   slots between rows -- 0..3 (skin), 7 and 8, and 11. The other nine, 4, 5,
   6, 9, 10 and 12..15, are byte-identical in all four rows, and they are
   exactly the eye and white colors: near-black 0x0002, the eye yellow
   0x04bd, the dark red 0x0862 and the four greys and whites. Copying a whole
   ROM row therefore cannot move an eye color, because the ROM authored the
   rows so it would not.

   AN AUTHORED .pal HAS NO SUCH GUARANTEE. A hand-written Yoshi palette
   writes all sixteen slots, so it moves the eyes along with the skin unless
   whoever wrote it left those nine alone. That is worth checking on every
   Yoshi change, and it is the one thing the four built-ins are free of. */

enum { PC_MARIO = 0, PC_LUIGI = 1, PC_WARIO = 2, PC_YOSHI = 3,
       PC_COUNT = 4, PC_MAX_FILES = 5 };

const char *const PC_KEY[PC_COUNT] = {
    "PaletteMario", "PaletteLuigi", "PaletteWario", "PaletteYoshi",
};
const char *const PC_WHO[PC_COUNT] = { "Mario", "Luigi", "Wario", "Yoshi" };

/* The files a key may touch. Yoshi's "head with cap" is yoshi_head.bmd and
   his "head without cap" is yoshi_head_fill.bmd; he has no dropped cap.
   Luigi carries two more because his body and capped head each have two
   texture variants a texture sequence swaps between -- recolor one of a pair
   and he flashes his old color on the frames the other is up. */
const char *const PC_FILES[PC_COUNT][PC_MAX_FILES] = {
    { "data/player/mario_model.bmd",
      "data/player/mario_head_cap.bmd",
      "data/player/mario_head_nocap.bmd", 0, 0 },
    { "data/player/luigi_model.bmd",
      "data/player/luigi_head_cap.bmd",
      "data/player/luigi_head_nocap.bmd",
      "data/player/L_tx_bodytrans.btp",
      "data/player/L_tx_headtrans.btp" },
    { "data/player/wario_model.bmd",
      "data/player/wario_head_cap.bmd",
      "data/player/wario_head_nocap.bmd", 0, 0 },
    { "data/player/yoshi_model.bmd",
      "data/player/yoshi_head.bmd",
      "data/player/yoshi_head_fill.bmd", 0, 0 },
};

const char *const YOSHI_BODY = "data/player/yoshi_model.bmd";
const char *const YOSHI_HEAD = "data/player/yoshi_head.bmd";
const char *const YOSHI_FILL = "data/player/yoshi_head_fill.bmd";
const char *const YOSHI_PAL = "yoshi_all_16p_pl";

unsigned g_pc_owned[PC_COUNT][PC_MAX_FILES];

struct PcPal {
    int active;
    u8 *blob;               /* the .pal bytes, or the synthesized color sets */
    struct PalPatch *patch; /* records, colors pointing into blob */
    unsigned npatch;
};
struct PcPal g_pc[PC_COUNT];
int g_pc_state;             /* 0 not tried, 1 loaded */

/* The boot latch for "is this mod on at all", so the filter's per-file cost
   stays one compare. */
int pc_any(void)
{
    static int v = -1;
    if (v < 0)
        v = host_setting_character_palette_any();
    return v;
}

/* Which character owns a resolved file id, or -1 for nobody's. */
int pc_owner(unsigned fileID)
{
    if (!fileID)
        return -1;
    for (int c = 0; c < PC_COUNT; ++c)
        for (int k = 0; k < PC_MAX_FILES; ++k)
            if (g_pc_owned[c][k] && g_pc_owned[c][k] == fileID)
                return c;
    return -1;
}

/* A BASENAME, not a path. The schema says a value names palettes/<v>.pal, so
   a value carrying a separator is a typo rather than a choice -- and reading
   through one would let settings.json reach anywhere on the disk. */
int pc_name_is_plain(const char *v)
{
    for (; *v; ++v)
        if (*v == '/' || *v == '\\' || *v == ':')
            return 0;
    return 1;
}

/* Build Yoshi's three records for VS row `row` out of the ROM's own body
   palette. 1 on success; 0 leaves Yoshi on the ROM and sets *why. */
int pc_build_yoshi(int row, const char **why)
{
    static const u16 kCount[3] = { 64, 16, 16 };
    static const u32 kOff[3] = { 0, 128, 160 };
    const char *kPath[3];
    u32 blen = 0, dp = 0, sz = 0;
    u8 *body, *syn;
    struct PalPatch *pp;

    kPath[0] = YOSHI_BODY;
    kPath[1] = YOSHI_HEAD;
    kPath[2] = YOSHI_FILL;

    *why = "yoshi_model.bmd is not in this dump";
    body = file_by_name(YOSHI_BODY, &blen);
    if (!body)
        return 0;
    *why = "yoshi_all_16p_pl is not the four stacked rows this build knows";
    if (!palette_named(body, blen, YOSHI_PAL, &dp, &sz) || sz != 128) {
        free(body);
        return 0;
    }
    *why = "out of host memory";
    syn = (u8 *)malloc(64 * 2 + 16 * 2 + 16 * 2);
    pp = (struct PalPatch *)calloc(3, sizeof *pp);
    if (!syn || !pp) {
        free(syn);
        free(pp);
        free(body);
        return 0;
    }
    /* the body keeps rows 1..3 exactly as authored and only row 0 moves;
       each head file gets the chosen row on its own, 16 colors, in place */
    memcpy(syn, body + dp, 128);
    memcpy(syn, body + dp + (u32)row * 32, 32);
    memcpy(syn + 128, body + dp + (u32)row * 32, 32);
    memcpy(syn + 160, body + dp + (u32)row * 32, 32);
    free(body);
    for (int i = 0; i < 3; ++i) {
        snprintf(pp[i].path, sizeof pp[i].path, "%s", kPath[i]);
        snprintf(pp[i].name, sizeof pp[i].name, "%s", YOSHI_PAL);
        pp[i].count = kCount[i];
        pp[i].colors = syn + kOff[i];
        pp[i].file_id = resolve_file_by_name(pp[i].path);
    }
    g_pc[PC_YOSHI].blob = syn;
    g_pc[PC_YOSHI].patch = pp;
    g_pc[PC_YOSHI].npatch = 3;
    return 1;
}

void pc_load(void)
{
    if (g_pc_state)
        return;
    g_pc_state = 1;

    for (int c = 0; c < PC_COUNT; ++c)
        for (int k = 0; k < PC_MAX_FILES; ++k)
            if (PC_FILES[c][k])
                g_pc_owned[c][k] = resolve_file_by_name(PC_FILES[c][k]);

    if (palette_combo())
        fprintf(stderr, "[mods] CustomPalette %d is off for this run: the "
                        "per-character palette keys are set, and two mods "
                        "writing the same palettes would make the colors "
                        "depend on which one ran last\n", palette_combo());

    for (int c = 0; c < PC_COUNT; ++c) {
        const char *v = host_setting_character_palette(c);
        char shown[1024];
        FILE *f;
        unsigned kept = 0, n;
        if (!*v)
            continue;

        if (c == PC_YOSHI) {
            /* host_settings.cpp owns the four spellings and hands over the
               row; a key that names a file answers -1 and falls through */
            const int row = host_setting_yoshi_builtin_row();
            if (row >= 0) {
                const char *why = "?";
                if (pc_build_yoshi(row, &why)) {
                    g_pc[c].active = 1;
                    fprintf(stderr, "[mods] PaletteYoshi %s: %u record(s) "
                                    "from the game's own four-player colors "
                                    "(row %d of %s)\n",
                            v, g_pc[c].npatch, row, YOSHI_PAL);
                } else {
                    fprintf(stderr, "[mods] PaletteYoshi %s REFUSED: %s; "
                                    "Yoshi keeps the ROM's colors\n", v, why);
                }
                continue;
            }
        }

        if (!pc_name_is_plain(v)) {
            fprintf(stderr, "[mods] %s %s: a palette name is a plain file "
                            "name, not a path; %s keeps the ROM's colors\n",
                    PC_KEY[c], v, PC_WHO[c]);
            continue;
        }
        f = palette_open_named(v, shown, sizeof shown);
        if (!f) {
            fprintf(stderr, "[mods] %s %s: no palettes/%s.pal beside the "
                            "exe, the asset root, or the working directory; "
                            "%s keeps the ROM's colors\n",
                    PC_KEY[c], v, v, PC_WHO[c]);
            continue;
        }
        n = palm_read(f, &g_pc[c].blob, &g_pc[c].patch);
        if (!n) {
            fprintf(stderr, "[mods] %s %s: %s is not a palmod blob this "
                            "build understands; %s keeps the ROM's colors\n",
                    PC_KEY[c], v, shown, PC_WHO[c]);
            continue;
        }
        /* keep only what is aimed at this character's own files, compacting
           in place so the filter loop has nothing to skip */
        for (unsigned r = 0; r < n; ++r) {
            struct PalPatch *p = &g_pc[c].patch[r];
            p->file_id = resolve_file_by_name(p->path);
            if (!p->file_id) {
                fprintf(stderr, "[mods] %s %s: %s is not in this dump; that "
                                "record is off for this run\n",
                        PC_KEY[c], v, p->path);
                continue;
            }
            if (pc_owner(p->file_id) != c)
                continue;   /* somebody else's file in a shared palette */
            if (kept != r)
                g_pc[c].patch[kept] = *p;
            ++kept;
        }
        g_pc[c].npatch = kept;
        g_pc[c].active = kept > 0;
        fprintf(stderr, "[mods] %s %s: %u of %u record(s) apply to %s (%s)\n",
                PC_KEY[c], v, kept, n, PC_WHO[c], shown);
    }
}

u32 character_palette_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!pc_any())
        return size;
    pc_load();
    for (int c = 0; c < PC_COUNT; ++c) {
        if (!g_pc[c].active)
            continue;
        for (unsigned i = 0; i < g_pc[c].npatch; ++i) {
            struct PalPatch *p = &g_pc[c].patch[i];
            const char *why;
            if (p->file_id != fileID)
                continue;
            why = palette_try(p, *data, size);
            if (p->said++)
                continue;
            if (why)
                fprintf(stderr, "[mods] %s: %s '%s' REFUSED: %s; served as "
                                "the ROM has it\n",
                        PC_KEY[c], p->path, p->name, why);
            else
                fprintf(stderr, "[mods] %s: %s '%s' recolored, %u colors\n",
                        PC_KEY[c], p->path, p->name, p->count);
        }
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

/* ---- THE FOURTH MOD: the lobby's per-player Yoshi colours -----------------
   SM64DS_VS_COLORS, four comma-separated fields in SLOT order, each field
   either empty or "bbbbbb:ssssss" -- a body colour and a shoe colour, six hex
   digits each. Empty field: that slot keeps the ROM's built-in colour.

   FOUR FIELDS TODAY, AND THE SHAPE PAST FOUR PLAYERS IS ALREADY DECIDED.
   Coordinator's cross-lane ruling of 2026-09-01, raised by SEAT16 and answered
   once for both variables: this mirrors SM64DS_VS_NAMES exactly -- four fields,
   exactly three commas, same discipline. When the wire itself supports more
   than four slots, BOTH variables move together, in one coordinated version
   change, to SIXTEEN comma-separated fields (exactly fifteen commas, same
   per-field grammar). Neither ever moves on its own. So a reader that grows to
   sixteen without SM64DS_VS_NAMES growing with it is a bug, not a head start.

   THE OWNER'S ORDER, his words: "the ability to choose a hex color to make
   your yoshi and its shoes so two [colors]. and the main color shows next to
   your name in lobby so people can identify you. It needs to go through the
   tangos sm64ds editions texture palette section, it will show you what parts
   of the texture you need to change and how the blending and shading works."
   The last sentence is the reason hal/vs_palette_gen.h exists and is the
   Studio's arithmetic rather than something quicker; this block only spends it.

   WHY THIS IS A FILE MOD AND NOT A RENDER HOOK. VS already picks a Yoshi's
   colour by pointing every one of his materials at a different SIXTEEN-COLOUR
   ROW of the same palette -- the ROM does it, and the port reproduces it in
   hal_player_vs_palette. So there is nothing to intercept at draw time: the
   four rows ARE the four players' colours, and the only thing a custom colour
   has to do is arrive in the file before the palette is uploaded. That is this
   seam, and it is the seam PaletteYoshi already uses for the built-ins.

   NOTHING ABOUT A COLOUR TRAVELS AS DATA. The lobby hands every launcher the
   same four hex pairs and every copy of the game builds all four rows for
   itself out of them. Identical inputs through identical arithmetic give
   identical bytes, so there is no palette on the wire to trust and nothing a
   hostile client can inject past six hex digits. It also cannot desync a
   match: this is a palette, it is decided before the level loads, and the
   simulation never reads it.

   THE VARIABLE IS NOT TRUSTED even though the lobby validates it and the
   launcher re-validates it. It is a string from another process, built from
   text a stranger typed into a room; every rule below is checked here as if
   neither of those steps had happened. A whole-variable grammar violation
   drops the variable WHOLESALE and every Yoshi keeps his built-in colour,
   which is the same contract SM64DS_VS_NAMES keeps (hal/star_flow.cpp:1012)
   and the reason every existing proof is unaffected: nothing that does not set
   the name can be moved by this. */

struct VsColorSpec {
    int set;
    u8  body[3];
    u8  shoes[3];
};

/* run vs16: sixteen, moving in the same commit set as SM64DS_VS_NAMES per the
   coordinator's ruling. See the banner above and hal/star_flow.cpp's copy of
   the same grammar note -- the two strings share one per-field grammar and one
   comma-count rule, which is the half of the ruling that stops two readers
   from splitting them slightly differently. */
VsColorSpec g_vsc[kPortMaxPlayers];
int g_vsc_fields;       /* 4 or 16, whichever the string carried */
int g_vsc_state;        /* 0 not read, 1 read */
int g_vsc_any;
u8 *g_vsc_blob;
struct PalPatch *g_vsc_patch;
unsigned g_vsc_npatch;

/* Parse one field. "" is a slot with no colour and is not an error; anything
   else must be exactly "bbbbbb:ssssss". 0 means the WHOLE variable is bad. */
int vsc_field(const char *f, unsigned n, VsColorSpec *out)
{
    out->set = 0;
    if (n == 0)
        return 1;
    if (n != 13 || f[6] != ':')
        return 0;
    if (!vspal::parse_hex6(f, out->body) || !vspal::parse_hex6(f + 7, out->shoes))
        return 0;
    out->set = 1;
    return 1;
}

void vsc_parse(void)
{
    const char *e = getenv("SM64DS_VS_COLORS");
    unsigned len, start, slot, commas;

    if (!e)
        return;                     /* absent: the common case, and silent */
    len = (unsigned)strlen(e);
    /* 16 x 13 + 15 = 223 at the wide shape, 55 at the narrow one. The comma
       count below decides which; this only has to not reject a legal one. */
    if (len < 3 || len > 223) {
        fprintf(stderr, "[mods] SM64DS_VS_COLORS ignored: %u bytes, the "
                        "contract allows 3..55 (four fields) or 3..223 "
                        "(sixteen)\n", len);
        return;
    }
    commas = 0;
    for (unsigned i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)e[i];
        if (c == ',') { ++commas; continue; }
        if (c != ':' && !((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
                          || (c >= 'A' && c <= 'F'))) {
            fprintf(stderr, "[mods] SM64DS_VS_COLORS ignored: byte %02x at %u "
                            "is not a hex digit, a colon or a comma\n",
                    (unsigned)c, i);
            return;
        }
    }
    if (commas == kPortNarrowPlayers - 1) {
        g_vsc_fields = kPortNarrowPlayers;
    } else if (commas == kPortMaxPlayers - 1) {
        g_vsc_fields = kPortMaxPlayers;
    } else {
        fprintf(stderr, "[mods] SM64DS_VS_COLORS ignored: %u comma(s), the "
                        "contract requires exactly 3 (four fields) or exactly "
                        "15 (sixteen)\n", commas);
        return;
    }
    start = 0;
    slot = 0;
    for (unsigned i = 0; i <= len; ++i) {
        if (i == len || e[i] == ',') {
            VsColorSpec s;
            if (!vsc_field(e + start, i - start, &s)) {
                fprintf(stderr, "[mods] SM64DS_VS_COLORS ignored: field %u is "
                                "neither empty nor bbbbbb:ssssss\n", slot);
                for (int k = 0; k < kPortMaxPlayers; ++k) g_vsc[k].set = 0;
                return;
            }
            g_vsc[slot++] = s;
            start = i + 1;
        }
    }
    for (int k = 0; k < g_vsc_fields; ++k)
        if (g_vsc[k].set)
            g_vsc_any = 1;
    if (!g_vsc_any) {
        fprintf(stderr, "[mods] SM64DS_VS_COLORS: every field is empty; every "
                        "Yoshi keeps his built-in colour\n");
        return;
    }
    for (int k = 0; k < g_vsc_fields; ++k) {
        if (!g_vsc[k].set)
            continue;
        fprintf(stderr, "[mods] SM64DS_VS_COLORS slot %d: body %02x%02x%02x "
                        "shoes %02x%02x%02x\n", k,
                g_vsc[k].body[0], g_vsc[k].body[1], g_vsc[k].body[2],
                g_vsc[k].shoes[0], g_vsc[k].shoes[1], g_vsc[k].shoes[2]);
    }
    /* AND THE ONE CEILING THIS LANE COULD NOT MOVE, said plainly rather than
       left to be discovered on screen.

       yoshi_all_16p_pl is 128 bytes: FOUR rows of sixteen colours, and the
       size is checked exactly (`sz != 128`) because the whole patch is built
       against that shape. The game reads one row per player out of that one
       palette. So above four players the rows are REUSED -- slot 4 wears slot
       0's colours, slot 5 wears slot 1's, and so on -- and there is nothing
       this file can do about it, because growing the palette means growing an
       asset the game allocates from its own data rather than a host array the
       port hosts.

       Sixteen authored palette rows are OWED, per status/VS16.md. Until they
       exist this is a cosmetic collision at five or more players, not a
       correctness one: identity is mPlayerNo and the name string, never the
       colour. */
    /* run pal16: THE WARNING THAT USED TO BE HERE IS RETIRED, and it was wrong
       twice over. It said "slot n wears slot (n mod 4)'s colours"; the actual
       reuse was slot n wearing slot ((n-1) mod 3) + 1's, because the spawn
       packs a cycled 1,2,3 into the two-bit flag field rather than n itself
       (hal/level_boot.cpp), so seat 4 drew seat 1's row and not seat 0's. Both
       halves are fixed now: level_boot repairs Player+0x61C to the true slot,
       and yoshi_rows16_filter below serves sixteen real rows. */
}

/* Build the patch records.
   run pal16: THE BODY RECORD IS GONE FROM HERE. It used to write the four
   64-word rows in place; the body palette is now GROWN to sixteen rows by
   yoshi_rows16_filter below, which runs last in the chain and reads this
   variable itself, so writing four rows here first would have meant applying
   the family transform twice to rows 0..3. What is left is the two head
   records: both head files get slot 0's SIXTEEN words, for the same reason
   PaletteYoshi gives them one. The heads carry their own copy of row 0, and
   although VS re-points every head material at the BODY palette (see
   hal/player_bridges.cpp's hal_player_vs_palette) so the copy is never sampled
   in a match, keeping them in step costs 32 bytes and means a custom colour
   cannot half-apply if a head is ever drawn from its own. */
void vsc_build(void)
{
    static const u16 kCount[2] = { 16, 16 };
    static const u32 kOff[2] = { 0, 32 };
    const char *kPath[2];
    u32 blen = 0, dp = 0, sz = 0;
    u8 *body;
    u8 *syn;
    struct PalPatch *pp;
    u16 rows[4][16];

    kPath[0] = YOSHI_HEAD;
    kPath[1] = YOSHI_FILL;

    body = file_by_name(YOSHI_BODY, &blen);
    if (!body) {
        fprintf(stderr, "[mods] SM64DS_VS_COLORS off: yoshi_model.bmd is not "
                        "in this dump; every Yoshi keeps his built-in colour\n");
        return;
    }
    if (!palette_named(body, blen, YOSHI_PAL, &dp, &sz) || sz != 128) {
        fprintf(stderr, "[mods] SM64DS_VS_COLORS off: yoshi_all_16p_pl is not "
                        "the four stacked rows this build knows; every Yoshi "
                        "keeps his built-in colour\n");
        free(body);
        return;
    }
    for (int r = 0; r < 4; ++r)
        for (int i = 0; i < 16; ++i)
            rows[r][i] = rd16(body, dp + (u32)r * 32 + (u32)i * 2);
    free(body);

    syn = (u8 *)malloc(16 * 2 + 16 * 2);
    pp = (struct PalPatch *)calloc(2, sizeof *pp);
    if (!syn || !pp) {
        free(syn);
        free(pp);
        fprintf(stderr, "[mods] SM64DS_VS_COLORS off: out of host memory\n");
        return;
    }
    {
        u16 out[16];
        if (g_vsc[0].set)
            vspal::vs_palette_row(rows[0], g_vsc[0].body, g_vsc[0].shoes, out);
        else
            memcpy(out, rows[0], 32);
        memcpy(syn, out, 32);           /* both heads take slot 0's row */
        memcpy(syn + 32, out, 32);
    }
    for (int i = 0; i < 2; ++i) {
        snprintf(pp[i].path, sizeof pp[i].path, "%s", kPath[i]);
        snprintf(pp[i].name, sizeof pp[i].name, "%s", YOSHI_PAL);
        pp[i].count = kCount[i];
        pp[i].colors = syn + kOff[i];
        pp[i].file_id = resolve_file_by_name(pp[i].path);
    }
    g_vsc_blob = syn;
    g_vsc_patch = pp;
    g_vsc_npatch = 2;
}

void vsc_load(void)
{
    if (g_vsc_state)
        return;
    g_vsc_state = 1;
    vsc_parse();
    if (!g_vsc_any)
        return;
    if (host_setting_character_palette_any()
        && *host_setting_character_palette(PC_YOSHI))
        fprintf(stderr, "[mods] PaletteYoshi is set and so is "
                        "SM64DS_VS_COLORS; the lobby's colours win for this "
                        "run, because they are what the other players in the "
                        "room are drawing\n");
    vsc_build();
}

/* Runs LAST in the chain, so a room's colours are the last word on Yoshi's
   palette whatever else is switched on. Every rule palette_try enforces still
   applies: the palette is found by its own name in the served file, the colour
   count must equal the BMD's exactly, nothing moves and nothing is
   reallocated, and a disagreement refuses out loud and serves the ROM. */
u32 vs_colors_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!getenv("SM64DS_VS_COLORS"))
        return size;                /* the byte-inert path, one getenv wide */
    vsc_load();
    for (unsigned i = 0; i < g_vsc_npatch; ++i) {
        struct PalPatch *p = &g_vsc_patch[i];
        const char *why;
        if (p->file_id != fileID)
            continue;
        why = palette_try(p, *data, size);
        if (p->said++)
            continue;
        if (why)
            fprintf(stderr, "[mods] SM64DS_VS_COLORS: %s '%s' REFUSED: %s; "
                            "that file keeps the ROM's colours\n",
                    p->path, p->name, why);
        else
            fprintf(stderr, "[mods] SM64DS_VS_COLORS: %s '%s' recoloured, "
                            "%u colours\n", p->path, p->name, p->count);
    }
    return size;
}

/* ---- SIXTEEN ROWS: growing the served Yoshi palette -----------------------
 *
 * THE CEILING THIS LIFTS, and it is the one the block above used to say could
 * not be lifted from here. yoshi_all_16p_pl is 128 bytes -- four sixteen-colour
 * rows -- and VS reads one row per player out of it, so above four players the
 * rows were reused and seat 4 wore another seat's colours. The old note said
 * "growing the palette means growing an asset the game allocates from its own
 * data rather than a host array the port hosts". That was true about a host
 * array and wrong about the seam: the fs filter takes `u8 **data`, so it may
 * REPLACE the buffer it is handed and return a new size, which lovesme_filter
 * has always done. Growing the file is therefore the port's to do.
 *
 * HOW, exactly, and why it is the smallest edit that works. A whole 512-byte
 * palette is APPENDED at the end of the file and the record is repointed at it
 * (record +4 = offset, +8 = size). Five hundred and twelve and not three
 * hundred and eighty-four: the cartridge's own 128 bytes cannot serve as the
 * first quarter of the new palette, because the bytes that follow them belong
 * to other records, so the four ROM rows are copied up with the twelve new ones
 * and the old 128 are left where they are. Nothing else in the BMD moves, so
 * every other offset in the file -- bones, display lists, textures, materials, the
 * bone-slot table -- still means what it meant, and bmd_parse's and
 * palette_named's bounds checks (`dp + sz <= len`) hold against the new length.
 * Rewriting the palette in place would have shifted every offset after it.
 *
 * AND THE RUNTIME FOLLOWS THE SIZE FIELD, end to end, with no clamp and no
 * fixed 128 anywhere between the record and the host memcpy. The upload walk is
 * src/_ZN5Model13LoadTexAndPalER8BMD_File.cpp, reached once per unique BMD from
 * Model::DoSetFile via Model::AddToCommonModelDataArr, and ONE `sz` read out of
 * the record drives the bounds test, the reservation and the copy length:
 *
 *     sz = p->size;
 *     if (data_020a4bcc + sz > data_020a4bd8) Crash();
 *     ...
 *     data_020a4bd8 -= ((sz + 0xf) & 0xfff0);
 *     GX::LoadTexPltt(p->data, data_020a4bd8, sz);
 *     p->vramOffset = data_020a4bd8;
 *
 * (Model::UpdateFileOffsets rebases the record's POINTERS on load and leaves
 * the size at +0x08 alone, so the grown size survives the rebase. 512 is over
 * the `sz <= 8` threshold, so it takes the falling arena from the 0x18000
 * ceiling, and 512 is already 16-aligned.)
 *
 * THE ROW THEN FALLS OUT OF THE UNITS. src/func_020462d0.c builds the runtime
 * material record and sets its +0x20 to `vramOffset >> 4` for a sixteen-colour
 * format, and ntr binds at PLTT_SLOT_BASE + base*16 (port/ntr/gx.cpp
 * bind_from_vram), so a material base is a palette address in sixteen-byte
 * units. Player+0x61C is base + (mPlayerNo << 1), and +2 in those units is +32
 * bytes, which is exactly one sixteen-colour row. Slot 15 lands at +480 of 512,
 * inside.
 *
 * NOT src/func_0204a028.c, which an earlier draft of this note cited: that is
 * the PARTICLE archive's palette walk (its only callers are
 * Particle::SysTracker::Initialise and one ov007 site) and no BMD goes through
 * it. It happens to be size-driven in the same way, which is how the wrong
 * citation survived reading.
 *
 * WHY IT IS UNCONDITIONAL rather than gated on the session width or on
 * SM64DS_VS_COLORS. Every peer has to agree about what everyone looks like, and
 * a gate is one more thing that can be true on one machine and false on
 * another. The file is served once per boot; the cost is 384 bytes of palette
 * VRAM out of the 0x18000-byte window, and rows 4..15 are unreachable unless a
 * player actually has mPlayerNo >= 4. Adventure, the minigames and every narrow
 * match bind rows 0..3 exactly as before.
 *
 * WHAT IT DOES NOT CHANGE. With no SM64DS_VS_COLORS, rows 0..3 come out of the
 * served buffer byte for byte, so the four ROM Yoshis are the cartridge's own
 * bytes. This runs LAST in mod_filter, after PaletteYoshi, CustomPalette and
 * the lobby's own colours, so whatever those wrote is the baseline the wide
 * rows are generated from rather than something they get to overwrite. */

int g_y16_said;

u8 *yoshi_rows16_grow(const u8 *d, u32 size, u32 *out_size, const char **why)
{
    u32 npal, palo, rec = 0, dp = 0, sz = 0, newoff, nsize;
    u16 rom[4][16], rows[16][16];
    vspal::VsPick picks[kPortMaxPlayers];
    u8 *n;

    *why = "file too small for a BMD header";
    if (size < 0x3c)
        return 0;
    npal = rd32(d, 0x1c);
    palo = rd32(d, 0x20);
    *why = "palette table is not a BMD's";
    if (npal == 0 || npal > 64 || palo > size || npal * 0x10 > size - palo)
        return 0;
    *why = "no yoshi_all_16p_pl in the file";
    for (u32 i = 0; i < npal; ++i) {
        u32 r = palo + i * 0x10, nm = rd32(d, r);
        if (nm < size && strcmp((const char *)d + nm, YOSHI_PAL) == 0) {
            rec = r;
            dp = rd32(d, r + 4);
            sz = rd32(d, r + 8);
            break;
        }
    }
    if (!rec)
        return 0;
    /* 128 is the shape this grows FROM. Anything else -- including 512, which
       would mean somebody already grew it -- is a disagreement rather than a
       state to be idempotent about, and is refused out loud like every other
       disagreement in this file. */
    *why = "yoshi_all_16p_pl is not the four stacked rows this build knows";
    if (sz != 128 || dp > size || sz > size - dp)
        return 0;

    for (int r = 0; r < 4; ++r)
        for (int i = 0; i < 16; ++i)
            rom[r][i] = rd16(d, dp + (u32)r * 32 + (u32)i * 2);

    vsc_load();
    for (int s = 0; s < kPortMaxPlayers; ++s) {
        picks[s].set = g_vsc[s].set;
        memcpy(picks[s].body, g_vsc[s].body, 3);
        memcpy(picks[s].shoes, g_vsc[s].shoes, 3);
    }
    vspal::vs_palette_rows16(rom, picks, rows);

    newoff = (size + 3u) & ~3u;
    nsize = newoff + 512u;
    *why = "out of host memory";
    n = (u8 *)malloc(nsize);
    if (!n)
        return 0;
    memcpy(n, d, size);
    memset(n + size, 0, newoff - size);
    for (int r = 0; r < 16; ++r)
        for (int i = 0; i < 16; ++i)
            wr16(n, newoff + (u32)r * 32 + (u32)i * 2, rows[r][i]);
    wr32(n, rec + 4, newoff);
    wr32(n, rec + 8, 512);
    *out_size = nsize;
    *why = 0;
    return n;
}

u32 yoshi_rows16_filter(unsigned fileID, u8 **data, u32 size)
{
    static unsigned id;
    static int resolved;
    const char *why = 0;
    u32 nsize = 0;
    u8 *n;

    if (!resolved) {
        resolved = 1;
        id = resolve_file_by_name(YOSHI_BODY);
    }
    if (!id || id != fileID)
        return size;

    n = yoshi_rows16_grow(*data, size, &nsize, &why);
    if (!n) {
        if (!g_y16_said++)
            fprintf(stderr, "[mods] sixteen Yoshi rows OFF: %s; the four ROM "
                            "rows are served and slots 4..15 share them\n",
                    why ? why : "refused");
        return size;
    }
    free(*data);
    *data = n;
    if (!g_y16_said++)
        fprintf(stderr, "[mods] yoshi_all_16p_pl grown to 16 rows (128 -> 512 "
                        "bytes at +%u); every VS seat has its own colours\n",
                nsize - 512);
    return nsize;
}

/* The installed filter is a chain, each mod deciding for itself whether the
   file is its business. Loves Me first because it can REPLACE the buffer;
   the palette patches then edit whatever bytes are actually being served.
   The two settings-driven palette mods are mutually exclusive by construction
   -- each one returns untouched when the other's keys are the ones set -- so
   their order decides nothing and neither can write over the other. The
   lobby's colours go last on purpose: a room full of people has to agree about
   what everyone looks like, so nothing on one player's disk may overrule it. */
u32 mod_filter(unsigned fileID, u8 **data, u32 size)
{
    size = lovesme_filter(fileID, data, size);
    size = palette_filter(fileID, data, size);
    size = character_palette_filter(fileID, data, size);
    size = vs_colors_filter(fileID, data, size);
    /* run pal16: LAST, and it must stay last. It reads the four rows out of
       whatever the chain above left in the buffer and generates twelve more
       from them, so anything that ran after it would be writing into a
       128-byte record that is no longer 128 bytes. */
    size = yoshi_rows16_filter(fileID, data, size);
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
