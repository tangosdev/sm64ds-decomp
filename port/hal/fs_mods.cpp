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
#include "vs_char_palette_gen.h"
#include "vs_char_groups.h"
#include "mods.h"
#include "mod_values.h"

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
extern void (*port_fs_mod_claims)(void (*claim)(unsigned fileID));
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

/* ---- THE TWO CATALOG MODS (run after093, lane CHARPALG1) ----------------
   "character-palettes" and "loves-me-swap" are the launcher's two old
   built-in rows moved into the mod catalog, so they can be updated without a
   game update. Each is a folder mod whose settings.json is the spec and whose
   settings.values.json holds the player's values (hal/mod_values.h reads the
   pair; a key missing from the values is the spec's default).

   WHEN THE MOD IS ON (in the list hal/mods.cpp latched from EnabledMods) its
   values are used INSTEAD of the old settings.json keys (PaletteMario,
   PaletteLuigi, PaletteWario, PaletteYoshi; LovesMeCharacter). When it is
   off, the old keys are read exactly as before, so an old launcher that still
   writes them keeps working. Read once, like every Mods key, and again after
   a relatch at the title (port_fs_mods_relatch_reset). */
const char *const CM_PALETTES = "character-palettes";
const char *const CM_LOVESME = "loves-me-swap";

/* The mod's index in the latched list, or -1 when it is not on. */
int cm_index(const char *id)
{
    for (int i = port_mods_count() - 1; i >= 0; --i) {
        const char *m = port_mods_id(i);
        if (m && strcmp(m, id) == 0)
            return i;
    }
    return -1;
}

/* A small text file of the mod's folder, NUL-terminated, or 0. */
char *cm_slurp(int mod, const char *name)
{
    char path[1100];
    const char *dir = port_mods_folder(mod);
    FILE *f;
    long n;
    char *t;
    size_t got;
    if (!dir)
        return 0;
    snprintf(path, sizeof path, "%s/%s", dir, name);
    f = fopen(path, "rb");
    if (!f)
        return 0;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0 || n > 256 * 1024) {
        fclose(f);
        return 0;
    }
    t = (char *)malloc((size_t)n + 1);
    if (!t) {
        fclose(f);
        return 0;
    }
    got = fread(t, 1, (size_t)n, f);
    fclose(f);
    t[got] = '\0';
    return t;
}

/* loves-me-swap's `character`, as host_setting_lovesme_character's 1-based
   index (0 = the ROM's Yoshi), or -1 when the mod is not on. */
int cm_lovesme(void)
{
    const int i = cm_index(CM_LOVESME);
    char v[64];
    char *spec, *vals;
    int src, idx;
    if (i < 0)
        return -1;
    spec = cm_slurp(i, "settings.json");
    vals = cm_slurp(i, "settings.values.json");
    src = modval::value(spec, vals, "character", v, sizeof v);
    free(spec);
    free(vals);
    idx = modval::lovesme_index(v);
    if (src && !idx)
        fprintf(stderr, "[mods] %s: character \"%s\" is not one this build "
                        "knows; the minigame keeps Yoshi\n", CM_LOVESME, v);
    fprintf(stderr, "[mods] %s on: character %s (%s); LovesMeCharacter is "
                    "not read\n", CM_LOVESME, idx ? v : "Yoshi",
            src == 2 ? "player's value" : src == 1 ? "default" : "none");
    return idx;
}

/* The boot latch: -1 until first asked, then the setting as it was then. */
int lovesme_character(void)
{
    static int v = -1;
    if (v < 0) {
        v = cm_lovesme();
        if (v < 0)
            v = host_setting_lovesme_character();
    }
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
    unsigned twin_said; /* same, once per balloon twin file (BALLOON_TWIN) */
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
    /* ENABLED MODS FIRST (run mods1, MODFILES1): <mod>/palettes/<name>.pal,
       the latest mod in EnabledMods first, then the usual places below. A mod
       that ships a palette under a name the player has chosen in the settings
       is the player's own choice of that mod, so it outranks the copy in the
       bundle's palettes/ folder. */
    for (int m = port_mods_count() - 1; m >= 0; --m) {
        snprintf(path, sizeof path, "%s/palettes/%s.pal", port_mods_folder(m), base);
        FILE *f = fopen(path, "rb");
        if (f) {
            snprintf(shown, cap, "%s", path);
            return f;
        }
    }
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

/* ---- BALLOON MARIO WEARS MARIO'S COLORS ----------------------------------
   The power flower's balloon Mario is drawn from three files of his own,
   data/player/b_mario_body.bmd, b_mario_head_cap.bmd and b_mario_all.bmd,
   not from mario_model.bmd and mario_head_cap.bmd. Their palettes are the
   regular files' palettes under the same names, byte for byte on the
   cartridge: b_mario_body's and b_mario_all's mario_body_pl are
   mario_model.bmd's mario_body_pl (336 colors), b_mario_head_cap's and
   b_mario_all's mario_head_pl are mario_head_cap.bmd's (256 colors), and
   b_mario_all's mario_eye_1_pl is mario_head_cap.bmd's (96 colors).
   b_mario_head_cap's mario_eye_0_pl is in no regular Mario file, so nothing
   reaches it this way. A palette mod authored against regular Mario
   therefore names only the regular files, and without this the balloon
   kept the cartridge's red and blue whatever Mario wore.

   So a record aimed at a regular file in the table below is applied to its
   balloon twin as well, through palette_try with every rule it has: the
   palette is found by its own name in the twin and the color count must
   match exactly. A palette the twin does not have is no twin and is
   skipped in silence (mario_head_cap's other eyes, say). A record in the
   same set that names the balloon file itself, for the same palette, wins:
   the authored answer beats the copy. The twin is CLAIMED for the card with
   its source (mod_claims), because the patched cartridge range is built from
   the claimed ids alone: an unclaimed twin is served straight off the
   cartridge and never reaches the filter. */
const char *const BALLOON_TWIN[][2] = {
    { "data/player/mario_model.bmd",    "data/player/b_mario_body.bmd" },
    { "data/player/mario_model.bmd",    "data/player/b_mario_all.bmd" },
    { "data/player/mario_head_cap.bmd", "data/player/b_mario_head_cap.bmd" },
    { "data/player/mario_head_cap.bmd", "data/player/b_mario_all.bmd" },
};
enum { BALLOON_TWINS = sizeof BALLOON_TWIN / sizeof BALLOON_TWIN[0] };
unsigned g_bt_id[BALLOON_TWINS][2];
int g_bt_state;

/* 1 when the served BMD has a palette of exactly this name. */
int palette_has_name(const u8 *d, u32 size, const char *name)
{
    u32 npal, palo;
    if (size < 0x3c)
        return 0;
    npal = rd32(d, 0x1c);
    palo = rd32(d, 0x20);
    if (npal > 64 || palo > size || npal * 0x10 > size - palo)
        return 0;
    for (u32 i = 0; i < npal; ++i) {
        u32 nm = rd32(d + palo + i * 0x10, 0);
        if (nm < size && str_at_is(d, size, nm, name))
            return 1;
    }
    return 0;
}

void balloon_twin_ids(void)
{
    if (g_bt_state)
        return;
    g_bt_state = 1;
    for (int t = 0; t < BALLOON_TWINS; ++t)
        for (int k = 0; k < 2; ++k)
            g_bt_id[t][k] = resolve_file_by_name(BALLOON_TWIN[t][k]);
}

/* Claim every balloon twin whose regular file a record of recs[0..n) names. */
void balloon_twin_claims(const struct PalPatch *recs, unsigned n,
                         void (*claim)(unsigned fileID))
{
    balloon_twin_ids();
    for (int t = 0; t < BALLOON_TWINS; ++t) {
        if (!g_bt_id[t][0] || !g_bt_id[t][1])
            continue;
        for (unsigned i = 0; i < n; ++i)
            if (recs[i].file_id == g_bt_id[t][0]) {
                claim(g_bt_id[t][1]);
                break;
            }
    }
}

/* Apply the records of recs[0..n) aimed at a regular Mario file to the
   served file when it is that file's balloon twin. `label` heads the log. */
void balloon_twins(struct PalPatch *recs, unsigned n, unsigned fileID,
                   u8 *d, u32 size, const char *label)
{
    balloon_twin_ids();
    for (int t = 0; t < BALLOON_TWINS; ++t) {
        if (!g_bt_id[t][0] || !g_bt_id[t][1] || g_bt_id[t][1] != fileID)
            continue;
        for (unsigned i = 0; i < n; ++i) {
            struct PalPatch *p = &recs[i];
            const char *why;
            int own = 0;
            if (p->file_id != g_bt_id[t][0] ||
                !palette_has_name(d, size, p->name))
                continue;
            for (unsigned j = 0; j < n && !own; ++j)
                own = recs[j].file_id == fileID &&
                      strcmp(recs[j].name, p->name) == 0;
            if (own)
                continue;
            why = palette_try(p, d, size);
            if (p->twin_said & (1u << t))
                continue;
            p->twin_said |= 1u << t;
            if (why)
                fprintf(stderr, "[mods] %s: %s '%s' for balloon Mario's %s "
                                "REFUSED: %s; served as the ROM has it\n",
                        label, p->path, p->name, BALLOON_TWIN[t][1], why);
            else
                fprintf(stderr, "[mods] %s: %s '%s' recolored on balloon "
                                "Mario's %s too, %u colors\n",
                        label, p->path, p->name, BALLOON_TWIN[t][1],
                        p->count);
        }
    }
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
    {
        char label[32];
        snprintf(label, sizeof label, "CustomPalette %d", palette_combo());
        balloon_twins(g_pal_patch, g_pal_npatch, fileID, *data, size, label);
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
   texture-swap files that drive his body and capped head, and for Mario the
   three balloon files the power flower draws him from (b_mario_body.bmd,
   b_mario_head_cap.bmd, b_mario_all.bmd). Balloon Mario IS Mario: a Mario
   palette reaches his balloon files through BALLOON MARIO WEARS MARIO'S
   COLORS above, and a .pal may also name a balloon file itself. Ownership is
   decided on the RESOLVED FILE ID, never on the spelling of a path, because
   a suffix test cannot tell data/player/mario_head_cap.bmd from
   data/player/b_mario_head_cap.bmd, and the two are different files with
   palettes of their own. A record whose
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
       PC_COUNT = 4, PC_MAX_FILES = 6 };

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
      "data/player/mario_head_nocap.bmd",
      "data/player/b_mario_body.bmd",
      "data/player/b_mario_head_cap.bmd",
      "data/player/b_mario_all.bmd" },
    { "data/player/luigi_model.bmd",
      "data/player/luigi_head_cap.bmd",
      "data/player/luigi_head_nocap.bmd",
      "data/player/L_tx_bodytrans.btp",
      "data/player/L_tx_headtrans.btp", 0 },
    { "data/player/wario_model.bmd",
      "data/player/wario_head_cap.bmd",
      "data/player/wario_head_nocap.bmd", 0, 0, 0 },
    { "data/player/yoshi_model.bmd",
      "data/player/yoshi_head.bmd",
      "data/player/yoshi_head_fill.bmd", 0, 0, 0 },
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

/* ---- character-palettes, the catalog mod (run after093, lane CHARPALG1) ---
   The same four answers as the keys above, read from the mod's settings
   instead (see THE TWO CATALOG MODS near the top of this file) when the mod
   is on: `mario` `luigi` `wario` `yoshi` each "Original", for Yoshi "Green"
   "Red" "Blue" "Yellow", a palette name (the mod's own palettes/<name>.pal
   first, through palette_open_named), or "Custom".

   CUSTOM is the player's own colours, `<who>_hat` `<who>_overalls`
   `<who>_shoes` `<who>_undershirt` for the bros and `yoshi_body` `yoshi_shoes`,
   "RRGGBB". PALSPLIT1 (11-13): the undershirt is its own key from package 1.2;
   until the player has a value of his own for it (an older package has no
   such key, and a 1.2 package's spec default is not his), the undershirt
   wears the hat's colour, as it did when the two were one pick. They land on
   exactly the palette entries an online pick lands on, with the same
   arithmetic: the bros through the category table hal/vs_char_groups.h and
   vspal::char_recolor_palette, group by group in the table's order, as
   hal/bro_colors.cpp's copy_for recolours a body's palette copy; Yoshi
   through vspal::vs_group_pick on the body group of row 0, as an online
   Yoshi's own row is built (vs_palette_row: vs_group_pick on the body
   group, then on the shoe group, both from row 0). A package older than the
   shoes setting has no `yoshi_shoes` key, so the shoes stay the cartridge's,
   as they always did. A colour equal to the cartridge's own tone of that part (to
   the DS's 5 bits a channel) is no pick, as it is online, so Custom with
   the default colours serves the ROM's palettes untouched.

   The difference from online is WHERE it is written: into the served file's
   palette, like every other palette mod here, because solo has one body per
   character file. That is also why Custom is SOLO ONLY: in a multiplayer
   run (port_mods_multiplayer) every body wearing the file would wear it, so
   Custom stands down and the room's picks (hal/bro_colors.cpp) are the only
   colours, exactly as with no palette chosen. */
struct CpMod {
    int state;              /* 0 not read, 1 read */
    int on;                 /* the mod is in the list */
    char choice[PC_COUNT][96];
    long rgb[PC_COUNT][4];  /* Custom colours, 0xRRGGBB; -1 = unset (part 3, the undershirt: unset wears the hat) */
};
struct CpMod g_cpm;
int g_pc_any = -1;

const char *const CP_CHOICE_KEY[PC_COUNT] = { "mario", "luigi", "wario", "yoshi" };
const char *const CP_PART_KEY[PC_COUNT][4] = {
    { "mario_hat", "mario_overalls", "mario_shoes", "mario_undershirt" },
    { "luigi_hat", "luigi_overalls", "luigi_shoes", "luigi_undershirt" },
    { "wario_hat", "wario_overalls", "wario_shoes", "wario_undershirt" },
    { "yoshi_body", "yoshi_shoes", 0, 0 },
};

void cp_mod_read(void)
{
    int i;
    char *spec, *vals;
    if (g_cpm.state)
        return;
    g_cpm.state = 1;
    for (int c = 0; c < PC_COUNT; ++c)
        for (int k = 0; k < 4; ++k)
            g_cpm.rgb[c][k] = -1;
    i = cm_index(CM_PALETTES);
    if (i < 0)
        return;
    g_cpm.on = 1;
    spec = cm_slurp(i, "settings.json");
    vals = cm_slurp(i, "settings.values.json");
    for (int c = 0; c < PC_COUNT; ++c) {
        modval::value(spec, vals, CP_CHOICE_KEY[c], g_cpm.choice[c],
                      sizeof g_cpm.choice[c]);
        for (int k = 0; k < 4; ++k) {
            char hex[16];
            int got;
            if (!CP_PART_KEY[c][k])
                continue;
            got = modval::value(spec, vals, CP_PART_KEY[c][k], hex, sizeof hex);
            if (k == 3 && got != 2)
                continue;               /* PALSPLIT1: not his own undershirt yet: it wears the hat */
            if (got)
                g_cpm.rgb[c][k] = modval::hex6(hex);
            if (g_cpm.rgb[c][k] < 0 && hex[0])
                fprintf(stderr, "[mods] %s: %s \"%s\" is not RRGGBB; that "
                                "part keeps the cartridge's colour\n",
                        CM_PALETTES, CP_PART_KEY[c][k], hex);
        }
    }
    free(spec);
    free(vals);
    fprintf(stderr, "[mods] %s on: Mario %s, Luigi %s, Wario %s, Yoshi %s; "
                    "the Palette keys are not read\n", CM_PALETTES,
            g_cpm.choice[0][0] ? g_cpm.choice[0] : "Original",
            g_cpm.choice[1][0] ? g_cpm.choice[1] : "Original",
            g_cpm.choice[2][0] ? g_cpm.choice[2] : "Original",
            g_cpm.choice[3][0] ? g_cpm.choice[3] : "Original");
}

/* The boot latch for "is this mod on at all", so the filter's per-file cost
   stays one compare. With character-palettes on, any choice but Original;
   otherwise any of the four keys, as before. */
int pc_any(void)
{
    if (g_pc_any < 0) {
        cp_mod_read();
        if (g_cpm.on) {
            int row;
            g_pc_any = 0;
            for (int c = 0; c < PC_COUNT; ++c)
                if (modval::choice_kind(g_cpm.choice[c], c == PC_YOSHI, &row)
                        != modval::CP_ORIGINAL)
                    g_pc_any = 1;
        } else {
            g_pc_any = host_setting_character_palette_any();
        }
    }
    return g_pc_any;
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
   palette. 1 on success; 0 leaves Yoshi on the ROM and sets *why.
   character-palettes' Custom passes `body_pick` and `shoe_pick` (0xRRGGBB,
   else -1): the row is then row 0 with its body group and its shoe group
   recoloured the way an online pick recolours them (vs_palette_row:
   vs_classic_alias, then vs_group_pick on the body group and on the shoe
   group, each reading row 0). A part whose colour is the cartridge's own
   drawn shade is no pick and keeps row 0's words, so Custom with both
   defaults serves the ROM untouched. */
static int pc_yoshi_part_pick(const u8 *pal, long rgb, int main_idx,
                              vspal::u8 alias[3])
{
    vspal::u8 pk[3];
    u16 m;
    if (rgb < 0)
        return 0;
    pk[0] = (vspal::u8)(rgb >> 16);
    pk[1] = (vspal::u8)(rgb >> 8);
    pk[2] = (vspal::u8)rgb;
    vspal::vs_classic_alias(pk, alias);
    memcpy(&m, pal + main_idx * 2, 2);
    return (vspal::rgb_word(alias, m) & 0x7fff) != (m & 0x7fff);
}

int pc_build_yoshi(int row, long body_pick, long shoe_pick, const char **why)
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
    vspal::u8 body_al[3], shoe_al[3];
    const int custom = body_pick >= 0 || shoe_pick >= 0;
    const int body_on = pc_yoshi_part_pick(body + dp, body_pick,
                                           vspal::kBodyMain, body_al);
    const int shoe_on = pc_yoshi_part_pick(body + dp, shoe_pick,
                                           vspal::kShoeMain, shoe_al);
    if (custom && !body_on && !shoe_on) {
        /* Custom colours the cartridge's own drawn green and orange: no pick */
        free(body);
        *why = "the body and shoe colours are the cartridge's own";
        return 2;
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
    if (custom) {
        u16 r0[16], out[16];
        memcpy(r0, body + dp, 32);
        memcpy(out, r0, 32);
        if (body_on)
            vspal::vs_group_pick(r0, vspal::kBodyIdx, 4, vspal::kBodyMain,
                                 body_al, out);
        if (shoe_on)
            vspal::vs_group_pick(r0, vspal::kShoeIdx, 2, vspal::kShoeMain,
                                 shoe_al, out);
        memcpy(syn, out, 32);
        memcpy(syn + 128, out, 32);
        memcpy(syn + 160, out, 32);
    } else {
        memcpy(syn, body + dp + (u32)row * 32, 32);
        memcpy(syn + 128, body + dp + (u32)row * 32, 32);
        memcpy(syn + 160, body + dp + (u32)row * 32, 32);
    }
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

/* What the log lines and the filter call each character's palette by: the
   old key's name, or the mod's setting. */
const char *g_pc_label[PC_COUNT] = {
    "PaletteMario", "PaletteLuigi", "PaletteWario", "PaletteYoshi",
};
const char *const CP_LOG_KEY[PC_COUNT] = {
    "character-palettes mario", "character-palettes luigi",
    "character-palettes wario", "character-palettes yoshi",
};

/* 0xRRGGBB to the DS's 5 bits a channel, for "is this the cartridge's own
   colour" (hal/bro_colors.cpp's q555). */
unsigned cp_q555(unsigned long rgb) { return (unsigned)(rgb & 0xF8F8F8u); }

/* A bro pick as it reaches the engine online: every pick travels as BGR555
   and comes back as 8 bits a channel with the top bits copied down
   (hal/comms_sync.cpp gw_bgr555 / gw_rgb_of555, "both sides go through
   BGR555"), so the recolour starts from that colour, not the typed one. */
unsigned long cp_wire555(unsigned long rgb)
{
    const unsigned r = (unsigned)(rgb >> 19) & 31, g = (unsigned)(rgb >> 11) & 31,
                   b = (unsigned)(rgb >> 3) & 31;
    return ((unsigned long)((r << 3) | (r >> 2)) << 16) |
           ((unsigned long)((g << 3) | (g >> 2)) << 8) | ((b << 3) | (b >> 2));
}

/* The picker row (0 hat, 1 overalls, 2 shoes, 3 undershirt) a category of
   hal/vs_char_groups.h answers to, or -1 (gloves, buttons: never picked). */
int cp_row_of_cat(int cat)
{
    if (cat == vscg::CAT_HAT) return 0;
    if (cat == vscg::CAT_OVERALLS) return 1;
    if (cat == vscg::CAT_SHOES) return 2;
    if (cat == vscg::CAT_UNDERSHIRT) return 3;
    return -1;
}

/* A bro's Custom colour for picker row r, the undershirt wearing the hat while
   it has none of its own (PALSPLIT1); -1 = unset. */
long cp_bro_rgb(int c, int r)
{
    if (r == 3 && g_cpm.rgb[c][3] < 0)
        r = 0;
    return g_cpm.rgb[c][r];
}

/* Custom for a bro: one record per (file, palette) the category table names
   for him with a real pick, each holding that palette's colours recoloured
   group by group in the table's order. 1 built, 2 nothing to recolour (every
   colour is the cartridge's), 0 refused with *why. */
int pc_build_bro(int c, const char **why)
{
    struct Tgt { const char *file, *pal; u8 *data; u32 dp, sz; } t[8];
    long pick[vscg::kVscGroupCount];
    int nt = 0;
    u32 total = 0, off = 0;
    u8 *syn;
    struct PalPatch *pp;

    for (int g = 0; g < vscg::kVscGroupCount; ++g) {
        const vscg::Group *G = &vscg::kVscGroups[g];
        const int r = G->chr == c ? cp_row_of_cat(G->cat) : -1;
        long v;
        int have = 0;
        pick[g] = -1;
        if (r < 0)
            continue;
        v = cp_bro_rgb(c, r);
        if (v < 0 || cp_q555((unsigned long)v) ==
                         cp_q555(vscg::kBroCart[c][G->cat]))
            continue;
        pick[g] = (long)cp_wire555((unsigned long)v);
        for (int k = 0; k < nt; ++k)
            if (!strcmp(t[k].file, G->file) && !strcmp(t[k].pal, G->pal))
                have = 1;
        if (!have && nt < 8) {
            t[nt].file = G->file;
            t[nt].pal = G->pal;
            t[nt].data = 0;
            ++nt;
        }
    }
    if (!nt) {
        *why = "every colour is the cartridge's own";
        return 2;
    }
    for (int k = 0; k < nt; ++k) {
        u32 len = 0;
        t[k].data = file_by_name(t[k].file, &len);
        if (!t[k].data ||
            !palette_named(t[k].data, len, t[k].pal, &t[k].dp, &t[k].sz) ||
            t[k].sz < 2 || t[k].sz > 0x2000 || (t[k].sz & 1)) {
            *why = t[k].data ? "a palette the colour table names is not in "
                               "its file as this build knows it"
                             : "a file the colour table names is not in this dump";
            for (int q = 0; q <= k; ++q)
                free(t[q].data);
            return 0;
        }
        total += t[k].sz;
    }
    syn = (u8 *)malloc(total);
    pp = (struct PalPatch *)calloc((size_t)nt, sizeof *pp);
    if (!syn || !pp) {
        free(syn);
        free(pp);
        for (int k = 0; k < nt; ++k)
            free(t[k].data);
        *why = "out of host memory";
        return 0;
    }
    for (int k = 0; k < nt; ++k) {
        vspal::u16 *w = (vspal::u16 *)(syn + off);
        memcpy(w, t[k].data + t[k].dp, t[k].sz);
        free(t[k].data);
        for (int g = 0; g < vscg::kVscGroupCount; ++g) {
            const vscg::Group *G = &vscg::kVscGroups[g];
            vspal::u8 rgb[3];
            const char *bad;
            if (pick[g] < 0 || strcmp(G->file, t[k].file) ||
                strcmp(G->pal, t[k].pal))
                continue;
            rgb[0] = (vspal::u8)(pick[g] >> 16);
            rgb[1] = (vspal::u8)(pick[g] >> 8);
            rgb[2] = (vspal::u8)pick[g];
            bad = vspal::char_recolor_palette(w, (int)(t[k].sz / 2), G->idx,
                                              G->count, rgb, G->main);
            if (bad)
                fprintf(stderr, "[mods] %s %s: %s\n", CP_LOG_KEY[c], G->pal,
                        bad);
        }
        snprintf(pp[k].path, sizeof pp[k].path, "%s", t[k].file);
        snprintf(pp[k].name, sizeof pp[k].name, "%s", t[k].pal);
        pp[k].count = (u16)(t[k].sz / 2);
        pp[k].colors = syn + off;
        pp[k].file_id = resolve_file_by_name(pp[k].path);
        off += t[k].sz;
    }
    g_pc[c].blob = syn;
    g_pc[c].patch = pp;
    g_pc[c].npatch = (unsigned)nt;
    return 1;
}

/* character-palettes' Custom for character c. Solo only: see the block
   comment at struct CpMod. */
void pc_custom(int c)
{
    const char *why = "?";
    int got;
    if (port_mods_multiplayer()) {
        fprintf(stderr, "[mods] %s Custom: off in multiplayer (solo only); "
                        "%s wears the room's colours\n", CP_LOG_KEY[c],
                PC_WHO[c]);
        return;
    }
    if (c == PC_YOSHI)
        got = pc_build_yoshi(0, g_cpm.rgb[c][0], g_cpm.rgb[c][1], &why);
    else
        got = pc_build_bro(c, &why);
    if (got == 1) {
        g_pc[c].active = 1;
        if (c == PC_YOSHI) {
            /* an older package has no yoshi_shoes: say "ROM", not FFFFFF */
            char sh[8];
            if (g_cpm.rgb[c][1] >= 0)
                snprintf(sh, sizeof sh, "%06lX",
                         (unsigned long)(g_cpm.rgb[c][1] & 0xFFFFFF));
            else
                snprintf(sh, sizeof sh, "ROM");
            fprintf(stderr, "[mods] %s Custom: body %06lX shoes %s, %u "
                            "record(s)\n", CP_LOG_KEY[c],
                    (unsigned long)(g_cpm.rgb[c][0] & 0xFFFFFF), sh,
                    g_pc[c].npatch);
        } else
            fprintf(stderr, "[mods] %s Custom: hat %06lX overalls %06lX "
                            "shoes %06lX undershirt %06lX%s, %u record(s)\n",
                    CP_LOG_KEY[c],
                    (unsigned long)(g_cpm.rgb[c][0] & 0xFFFFFF),
                    (unsigned long)(g_cpm.rgb[c][1] & 0xFFFFFF),
                    (unsigned long)(g_cpm.rgb[c][2] & 0xFFFFFF),
                    (unsigned long)(cp_bro_rgb(c, 3) & 0xFFFFFF),
                    g_cpm.rgb[c][3] < 0 ? " (the hat's)" : "",
                    g_pc[c].npatch);
    } else if (got == 2) {
        fprintf(stderr, "[mods] %s Custom: %s; %s keeps the ROM's colors\n",
                CP_LOG_KEY[c], why, PC_WHO[c]);
    } else {
        fprintf(stderr, "[mods] %s Custom REFUSED: %s; %s keeps the ROM's "
                        "colors\n", CP_LOG_KEY[c], why, PC_WHO[c]);
    }
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

    cp_mod_read();
    for (int c = 0; c < PC_COUNT; ++c) {
        const char *v;
        const char *key = PC_KEY[c];
        char shown[1024];
        FILE *f;
        unsigned kept = 0, n;
        int row = -1;

        g_pc_label[c] = PC_KEY[c];
        if (g_cpm.on) {
            /* character-palettes: its choice replaces the key outright */
            int kind;
            v = g_cpm.choice[c];
            key = CP_LOG_KEY[c];
            g_pc_label[c] = CP_LOG_KEY[c];
            kind = modval::choice_kind(v, c == PC_YOSHI, &row);
            if (kind == modval::CP_ORIGINAL)
                continue;
            if (kind == modval::CP_CUSTOM) {
                pc_custom(c);
                continue;
            }
        } else {
            v = host_setting_character_palette(c);
            if (!*v)
                continue;
            /* host_settings.cpp owns the four spellings and hands over the
               row; a key that names a file answers -1 and falls through */
            if (c == PC_YOSHI)
                row = host_setting_yoshi_builtin_row();
        }

        if (c == PC_YOSHI && row >= 0) {
            const char *why = "?";
            if (pc_build_yoshi(row, -1, -1, &why) == 1) {
                g_pc[c].active = 1;
                fprintf(stderr, "[mods] %s %s: %u record(s) "
                                "from the game's own four-player colors "
                                "(row %d of %s)\n",
                        key, v, g_pc[c].npatch, row, YOSHI_PAL);
            } else {
                fprintf(stderr, "[mods] %s %s REFUSED: %s; "
                                "Yoshi keeps the ROM's colors\n", key, v, why);
            }
            continue;
        }

        if (!pc_name_is_plain(v)) {
            fprintf(stderr, "[mods] %s %s: a palette name is a plain file "
                            "name, not a path; %s keeps the ROM's colors\n",
                    key, v, PC_WHO[c]);
            continue;
        }
        f = palette_open_named(v, shown, sizeof shown);
        if (!f) {
            fprintf(stderr, "[mods] %s %s: no palettes/%s.pal in an enabled mod, beside the "
                            "exe, the asset root, or the working directory; "
                            "%s keeps the ROM's colors\n",
                    key, v, v, PC_WHO[c]);
            continue;
        }
        n = palm_read(f, &g_pc[c].blob, &g_pc[c].patch);
        if (!n) {
            fprintf(stderr, "[mods] %s %s: %s is not a palmod blob this "
                            "build understands; %s keeps the ROM's colors\n",
                    key, v, shown, PC_WHO[c]);
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
                        key, v, p->path);
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
                key, v, kept, n, PC_WHO[c], shown);
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
                        g_pc_label[c], p->path, p->name, why);
            else
                fprintf(stderr, "[mods] %s: %s '%s' recolored, %u colors\n",
                        g_pc_label[c], p->path, p->name, p->count);
        }
        if (c == PC_MARIO)
            balloon_twins(g_pc[c].patch, g_pc[c].npatch, fileID, *data, size,
                          g_pc_label[c]);
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

/* ---- THE FIFTH MOD: the lobby's per-CHARACTER, per-CATEGORY bro colours ----
   SM64DS_VS_CHAR_COLORS -- the game side of recolouring the bros (Mario, Luigi,
   Wario) in VS by category. It mirrors SM64DS_VS_COLORS / SM64DS_VS_CHARS: comma
   separated fields in SLOT order, EXACTLY 3 commas (four fields) OR 15 (sixteen).
   Each field is either empty (that slot's character keeps its ROM colours) or a
   run of FIVE or SIX colon-separated categories, in Tango's order:

       hat : overalls : gloves : buttons : shoes [ : undershirt ]

   so four or five colons per non-empty field. Five is the grammar from before
   PALSPLIT1 (11-13), when the hat and the undershirt were one category: the hat
   field then recolours both, as it did. With six, the undershirt is its own. Each category is empty (leave that
   part on the ROM) or six hex digits (recolour it toward that colour). Example,
   a Mario slot recolouring only overalls green and gloves red:
       ":00ff00:ff0000::"
   A whole field ":", i.e. all five empty, is legal and recolours nothing.

   HAT AND UNDERSHIRT ARE TWO CATEGORIES since PALSPLIT1 (they were one, Tango's
   tie of 10-13, until he asked for them apart on 11-13). The hat lives in the
   head BMD and the undershirt in the body BMD; each picked colour drives a
   family transform in its own file, shaded from that file's main tone. The membership
   is vs_char_groups.h, generated from the cartridge files themselves
   (port/tools/gen_vs_char_groups.py: every entry of every bro palette
   classified by colour and by where its texels sit; run after093 BROCOLOR1).
   Gloves and buttons have no entries: those two fields are accepted and do
   nothing.

   WHY THIS IS A FILE MOD LIKE THE OTHERS. Each bro model is a served BMD with a
   named palette; recolouring by category is palette_try's shape -- find the
   named palette, recolour its indices, write the words back in place, no resize.
   The arithmetic is vs_palette_gen.h's verified family transform, spent through
   vs_char_palette_gen.h. It runs after char-apply (player_bridges.cpp seats the
   character at frame 90); the palette is read when the model is served, before
   it is uploaded, so the recolour just has to be in the bytes by then.

   THE ONE HONEST LIMIT: keyed per CHARACTER, not per SLOT. The cartridge tells
   VS Yoshis apart by palette ROW, so two Yoshis can differ; the bros have ONE
   palette per model file and no per-seat row, so two slots both playing Mario
   share one mario_model.bmd and cannot wear different colours. The colours of
   the LOWEST slot that picked a character win for that character's file, and a
   second slot picking the same character with different colours is logged as a
   conflict. In the common VS arena (Mario/Luigi/Wario/Yoshi, each once) there is
   no conflict. Which slot owns which character comes from SM64DS_VS_CHARS; with
   no SM64DS_VS_CHARS no bro is seated, so this whole path is inert.

   IGNORED WHOLESALE on any grammar violation, the sibling contracts' rule. */

struct VccSpec {
    int set[vscg::CAT_COUNT];       /* which categories this slot picked */
    u8  col[vscg::CAT_COUNT][3];    /* the picked colour per category */
    int any;
};

VccSpec g_vcc[kPortMaxPlayers];
int g_vcc_char[kPortMaxPlayers];    /* engine char 0..2, or -1 = Yoshi/none */
int g_vcc_char_slot[3];             /* per bro chr, the winning slot, or -1 */
int g_vcc_fields;
int g_vcc_state;                    /* 0 not read, 1 read */
int g_vcc_any;

/* Parse SM64DS_VS_CHARS into g_vcc_char (engine 0..2, -1 for Yoshi/empty). Same
   grammar as the reader in player_bridges.cpp: '0'/empty = Yoshi (-1), 1..3 map
   to engine 0..2. A violation leaves every slot at -1 (all Yoshi). This is read
   here rather than shared because that reader is a static in another TU; the
   grammar is four bytes long and duplicating it costs less than a new export. */
void vcc_parse_chars(void)
{
    for (int i = 0; i < kPortMaxPlayers; ++i)
        g_vcc_char[i] = -1;
    const char *e = getenv("SM64DS_VS_CHARS");
    if (!e)
        return;
    unsigned len = (unsigned)strlen(e);
    if (len < 1 || len > 31)
        return;
    unsigned commas = 0;
    for (unsigned i = 0; i < len; ++i)
        if (e[i] == ',') ++commas;
    if (commas != (unsigned)(kPortNarrowPlayers - 1) &&
        commas != (unsigned)(kPortMaxPlayers - 1))
        return;
    int tmp[kPortMaxPlayers];
    for (int i = 0; i < kPortMaxPlayers; ++i) tmp[i] = -1;
    int slot = 0, w = 0;
    for (unsigned i = 0; i <= len; ++i) {
        char ch = (i < len) ? e[i] : ',';
        if (ch == ',') { ++slot; w = 0; continue; }
        if (++w > 1) return;                 /* field longer than one char */
        if (ch < '0' || ch > '3') return;    /* not 0..3 */
        if (slot < kPortMaxPlayers)
            tmp[slot] = (ch == '0') ? -1 : (ch - '1');
    }
    for (int i = 0; i < kPortMaxPlayers; ++i)
        g_vcc_char[i] = tmp[i];
}

/* Parse one SM64DS_VS_CHAR_COLORS field into a VccSpec. "" is a slot with no
   recolour and is not an error. Otherwise five or six colon-separated
   categories, each "" or six hex digits; five gives the undershirt the hat's
   (the grammar from before the split). 0 means the WHOLE variable is bad. */
int vcc_field(const char *f, unsigned n, VccSpec *out)
{
    for (int c = 0; c < vscg::CAT_COUNT; ++c) out->set[c] = 0;
    out->any = 0;
    if (n == 0)
        return 1;
    /* split on ':' -- must be exactly CAT_COUNT parts */
    unsigned start = 0, part = 0;
    for (unsigned i = 0; i <= n; ++i) {
        if (i == n || f[i] == ':') {
            unsigned plen = i - start;
            if (part >= (unsigned)vscg::CAT_COUNT)
                return 0;                    /* too many colons */
            if (plen != 0) {
                if (plen != 6)
                    return 0;
                if (!vspal::parse_hex6(f + start, out->col[part]))
                    return 0;
                out->set[part] = 1;
                out->any = 1;
            }
            ++part;
            start = i + 1;
        }
    }
    if (part == (unsigned)vscg::CAT_COUNT - 1) {
        /* PALSPLIT1: the five-field grammar: the hat field is the undershirt too */
        out->set[vscg::CAT_UNDERSHIRT] = out->set[vscg::CAT_HAT];
        for (int k = 0; k < 3; ++k)
            out->col[vscg::CAT_UNDERSHIRT][k] = out->col[vscg::CAT_HAT][k];
        return 1;
    }
    if (part != (unsigned)vscg::CAT_COUNT)
        return 0;                            /* too few colons */
    return 1;
}

void vcc_parse(void)
{
    static const char *const kCat[vscg::CAT_COUNT] = {
        "hat", "overalls", "gloves", "buttons", "shoes", "undershirt"
    };
    static const char *const kName[3] = { "Mario", "Luigi", "Wario" };
    const char *e = getenv("SM64DS_VS_CHAR_COLORS");
    unsigned len, start, slot, commas;

    for (int i = 0; i < 3; ++i) g_vcc_char_slot[i] = -1;

    if (!e)
        return;                              /* absent: common case, silent */
    len = (unsigned)strlen(e);
    /* widest field is 6*6 + 5 colons = 41; sixteen of them plus 15 commas is
       671. narrow is 4*41 + 3 = 167. the comma count decides which. */
    if (len < 1 || len > 671) {
        fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS ignored: %u bytes, out of "
                        "the contract's range\n", len);
        return;
    }
    commas = 0;
    for (unsigned i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)e[i];
        if (c == ',') { ++commas; continue; }
        if (c != ':' && !((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
                          || (c >= 'A' && c <= 'F'))) {
            fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS ignored: byte %02x at "
                            "%u is not a hex digit, a colon or a comma\n",
                    (unsigned)c, i);
            return;
        }
    }
    if (commas == (unsigned)(kPortNarrowPlayers - 1))
        g_vcc_fields = kPortNarrowPlayers;
    else if (commas == (unsigned)(kPortMaxPlayers - 1))
        g_vcc_fields = kPortMaxPlayers;
    else {
        fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS ignored: %u comma(s), the "
                        "contract requires exactly 3 (four fields) or exactly "
                        "15 (sixteen)\n", commas);
        return;
    }

    start = 0;
    slot = 0;
    for (unsigned i = 0; i <= len; ++i) {
        if (i == len || e[i] == ',') {
            VccSpec s;
            if (!vcc_field(e + start, i - start, &s)) {
                fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS ignored: field %u "
                                "is neither empty nor five or six colon-separated "
                                "categories (hat:overalls:gloves:buttons:shoes"
                                "[:undershirt])\n", slot);
                for (int k = 0; k < kPortMaxPlayers; ++k) g_vcc[k].any = 0;
                return;
            }
            if (slot < (unsigned)kPortMaxPlayers)
                g_vcc[slot] = s;
            ++slot;
            start = i + 1;
        }
    }

    vcc_parse_chars();

    for (int k = 0; k < g_vcc_fields; ++k)
        if (g_vcc[k].any)
            g_vcc_any = 1;
    if (!g_vcc_any) {
        fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS: every field is empty; "
                        "every bro keeps his ROM colours\n");
        return;
    }

    /* Resolve, per bro character, the winning slot: the LOWEST slot that both
       picked that character (SM64DS_VS_CHARS) and set at least one category. */
    for (int chr = 0; chr < 3; ++chr) {
        for (int k = 0; k < g_vcc_fields; ++k) {
            if (g_vcc_char[k] == chr && g_vcc[k].any) {
                g_vcc_char_slot[chr] = k;
                break;
            }
        }
        /* a conflict is a second slot playing the same character with any
           category set -- logged, but the lowest slot still wins the file */
        if (g_vcc_char_slot[chr] >= 0) {
            for (int k = g_vcc_char_slot[chr] + 1; k < g_vcc_fields; ++k)
                if (g_vcc_char[k] == chr && g_vcc[k].any)
                    fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS: slots %d and "
                            "%d both play %s; the bros share one palette file, "
                            "so slot %d's colours win for both\n",
                            g_vcc_char_slot[chr], k, kName[chr],
                            g_vcc_char_slot[chr]);
        }
    }

    for (int chr = 0; chr < 3; ++chr) {
        int s = g_vcc_char_slot[chr];
        if (s < 0)
            continue;
        fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS %s (slot %d):", kName[chr],
                s);
        int printed = 0;
        for (int c = 0; c < vscg::CAT_COUNT; ++c)
            if (g_vcc[s].set[c]) {
                fprintf(stderr, " %s=%02x%02x%02x", kCat[c], g_vcc[s].col[c][0],
                        g_vcc[s].col[c][1], g_vcc[s].col[c][2]);
                printed = 1;
            }
        if (!printed)
            fprintf(stderr, " (no category set)");
        fprintf(stderr, "\n");
    }
}

/* Resolve the group table's file paths to this dump's file ids, once. */
unsigned g_vcc_group_id[64];
int g_vcc_ids_done;
void vcc_resolve_ids(void)
{
    if (g_vcc_ids_done)
        return;
    g_vcc_ids_done = 1;
    for (int i = 0; i < vscg::kVscGroupCount && i < 64; ++i)
        g_vcc_group_id[i] = resolve_file_by_name(vscg::kVscGroups[i].file);
}

void vcc_load(void)
{
    if (g_vcc_state)
        return;
    g_vcc_state = 1;
    vcc_parse();
    if (!g_vcc_any)
        return;
    vcc_resolve_ids();
}

/* Recolour one category group's indices in the served BMD, in place. Returns 0
   and writes the changed words on success, or a reason string (writing nothing)
   otherwise. The named palette is found the same way palette_try finds it. */
const char *vcc_recolor_group(u8 *d, u32 size, const vscg::Group *g,
                              const u8 target[3])
{
    u32 dp = 0, sz = 0;
    if (!palette_named(d, size, g->pal, &dp, &sz))
        return "no palette of that name in the file";
    int ncol = (int)(sz / 2);
    if (ncol <= 0)
        return "empty palette";
    u16 *buf = (u16 *)malloc((size_t)ncol * 2);
    if (!buf)
        return "out of host memory";
    for (int i = 0; i < ncol; ++i)
        buf[i] = rd16(d, dp + (u32)i * 2);
    /* the pick lands on the shade this file draws most: counted by the
       generator from the cartridge textures (vs_char_groups.h `main`) */
    (void)size;
    const int main_idx = g->main;
    const char *why = vspal::char_recolor_palette(buf, ncol, g->idx, g->count,
                                                  target, main_idx);
    if (why) {
        free(buf);
        return why;
    }
    for (int i = 0; i < g->count; ++i)
        wr16(d, dp + (u32)g->idx[i] * 2, buf[g->idx[i]]);
    free(buf);
    return 0;
}

/* Runs in the chain like the other palette mods: for a served bro BMD, recolour
   every category the winning slot for that character set. Byte-inert with no
   SM64DS_VS_CHAR_COLORS (one getenv). */
int g_vcc_said[64];

u32 vs_char_colors_filter(unsigned fileID, u8 **data, u32 size)
{
    if (!getenv("SM64DS_VS_CHAR_COLORS"))
        return size;                         /* the byte-inert path */
    vcc_load();
    if (!g_vcc_any)
        return size;
    for (int i = 0; i < vscg::kVscGroupCount && i < 64; ++i) {
        const vscg::Group *g = &vscg::kVscGroups[i];
        if (!g_vcc_group_id[i] || g_vcc_group_id[i] != fileID)
            continue;
        int s = g_vcc_char_slot[g->chr];
        if (s < 0)
            continue;                        /* no slot plays this character */
        if (!g_vcc[s].set[g->cat])
            continue;                        /* this category not picked */
        const char *why = vcc_recolor_group(*data, size, g, g_vcc[s].col[g->cat]);
        if (g_vcc_said[i]++)
            continue;
        static const char *const kName[3] = { "Mario", "Luigi", "Wario" };
        static const char *const kCat[vscg::CAT_COUNT] = {
            "hat", "overalls", "gloves", "buttons", "shoes", "undershirt"
        };
        if (why)
            fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS: %s %s '%s' REFUSED: "
                    "%s; that palette keeps the ROM's colours\n", kName[g->chr],
                    kCat[g->cat], g->pal, why);
        else
            fprintf(stderr, "[mods] SM64DS_VS_CHAR_COLORS: %s %s '%s' recoloured,"
                    " %d index(es)\n", kName[g->chr], kCat[g->cat], g->pal,
                    g->count);
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
    /* HUB32: 32 rows, one per body; rows 16..31 repeat 0..15. JOINWIN1: plus row 32, a copy of row 0, which no body number names: the local
       player's own Adventure Online pick is drawn from it (hal/comms_sync.cpp port_adventure_local_palette), since rows 0..31 all belong to bodies */
    nsize = newoff + 1056u;
    *why = "out of host memory";
    n = (u8 *)malloc(nsize);
    if (!n)
        return 0;
    memcpy(n, d, size);
    memset(n + size, 0, newoff - size);
    for (int r = 0; r < 33; ++r)
        for (int i = 0; i < 16; ++i)
            wr16(n, newoff + (u32)r * 32 + (u32)i * 2, rows[r & 15][i]);   /* row 32 & 15 = row 0 */
    wr32(n, rec + 4, newoff);
    wr32(n, rec + 8, 1056);
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
                nsize - 1056);
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
    /* The lobby's per-character bro colours: a served bro BMD gets its
       categories recoloured after any local PaletteMario/etc, so the room wins.
       Yoshi's file is untouched here (this only knows the three bros), so it is
       order-independent with the Yoshi colour filters that follow. */
    size = vs_char_colors_filter(fileID, data, size);
    size = vs_colors_filter(fileID, data, size);
    /* run pal16: LAST, and it must stay last. It reads the four rows out of
       whatever the chain above left in the buffer and generates twelve more
       from them, so anything that ran after it would be writing into a
       128-byte record that is no longer 128 bytes. */
    size = yoshi_rows16_filter(fileID, data, size);
    return size;
}

/* EVERY FILE AN ACTIVE MOD ABOVE CAN CHANGE, named to the card (run linkfull,
   lane FILEB). The game targets link the ROM's own SharedFilePtr::Load, which
   reads a loose file off the card and an archive member out of the NARC the
   ROM mounted off the card, so a mod's bytes have to be ON the card: hal/fs.cpp
   (port_fs_card_patches) runs each id claimed here through mod_map and
   mod_filter once, at the first card read, and the card serves the result out
   of its patched range (a member inside its rebuilt NARC). Each claim sits
   under the gate its filter checks, so a mod that is off claims nothing; a
   claim whose served bytes come out the same as the cartridge's is dropped
   there, so claiming a file a filter may leave alone costs nothing. The
   narrow harnesses still link hal/fs.cpp's host Load, which runs the same two
   hooks per load and never asks for claims. */
void mod_claims(void (*claim)(unsigned fileID))
{
    unsigned i;
    if (lovesme_character()) {
        resolve_ids();
        if (g_yoshi_model && g_mario_model) {
            claim(g_yoshi_model);
            for (int a = 0; a < ANIM_COUNT; ++a)
                if (g_anims[a])
                    claim(g_anims[a]);
        }
    }
    if (!pc_any() && palette_combo()) {
        palette_load();
        if (g_pal_state > 0) {
            for (i = 0; i < g_pal_npatch; ++i)
                if (g_pal_patch[i].file_id)
                    claim(g_pal_patch[i].file_id);
            balloon_twin_claims(g_pal_patch, g_pal_npatch, claim);
        }
    }
    if (pc_any()) {
        pc_load();
        for (int c = 0; c < PC_COUNT; ++c)
            if (g_pc[c].active) {
                for (i = 0; i < g_pc[c].npatch; ++i)
                    if (g_pc[c].patch[i].file_id)
                        claim(g_pc[c].patch[i].file_id);
                if (c == PC_MARIO)
                    balloon_twin_claims(g_pc[c].patch, g_pc[c].npatch, claim);
            }
    }
    if (getenv("SM64DS_VS_CHAR_COLORS")) {
        vcc_load();
        if (g_vcc_any)
            for (int g = 0; g < vscg::kVscGroupCount && g < 64; ++g)
                if (g_vcc_group_id[g])
                    claim(g_vcc_group_id[g]);
    }
    if (getenv("SM64DS_VS_COLORS")) {
        vsc_load();
        for (i = 0; i < g_vsc_npatch; ++i)
            if (g_vsc_patch[i].file_id)
                claim(g_vsc_patch[i].file_id);
    }
    {   /* the sixteen Yoshi rows: unconditional, like yoshi_rows16_filter */
        const unsigned y = resolve_file_by_name(YOSHI_BODY);
        if (y)
            claim(y);
    }
}

/* Installed before main by the CRT's initializer walk; every fs call the
   game makes happens far later. Targets that do not link this file keep the
   null hooks. hal/stage_geom.cpp chains onto the filter and the claims from
   .CRT$XCV, after this. */
struct InstallHooks {
    InstallHooks()
    {
        port_fs_mod_map = mod_map;
        port_fs_mod_filter = mod_filter;
        port_fs_mod_claims = mod_claims;
    }
} g_install;

} /* namespace */

/* THE RE-LATCH'S HALF HERE (run after093, lane SEAMMODS1). The two palette
   mods read their .pal once, and palette_open_named looks in the ENABLED MODS'
   palettes/ first, so a mod list switched at the title (hal/mods.cpp
   port_mods_relatch_at_title) must let them read again: both states go back to
   "not tried" and their blobs are freed. They re-read on the card claims that
   follow, exactly as at boot. Everything else here is settings-driven (Loves
   Me, the lobby colours) or unconditional (the sixteen Yoshi rows) and the
   claims re-run it the same way the first latch did. */
extern "C" void port_fs_mods_relatch_reset(void)
{
    palette_release();
    g_pal_state = 0;
    for (int c = 0; c < PC_COUNT; ++c) {
        free(g_pc[c].blob);
        free(g_pc[c].patch);
        g_pc[c].blob = 0;
        g_pc[c].patch = 0;
        g_pc[c].npatch = 0;
        g_pc[c].active = 0;
    }
    g_pc_state = 0;
    /* CHARPALG1: character-palettes is read again from the new list (and
       Custom asks again whether this run is multiplayer) */
    memset(&g_cpm, 0, sizeof g_cpm);
    g_pc_any = -1;
    /* SEAMCLEAN1: the lobby colours are read once per process (vsc_load); a hand-off that brings new ones after an earlier
       match must read them again, so the state goes back to "not read" and the patch is freed, like the two palette mods above */
    free(g_vsc_blob);
    free(g_vsc_patch);
    g_vsc_blob = 0;
    g_vsc_patch = 0;
    g_vsc_npatch = 0;
    g_vsc_any = 0;
    g_vsc_fields = 0;
    memset(g_vsc, 0, sizeof g_vsc);
    g_vsc_state = 0;
}

/* BROSCOLOR1 (run coopfix1): the character-palettes mod's Custom colour for one bro part (chr 0 Mario 1 Luigi 2 Wario; part 0 HAT
   1 OVERALLS 2 SHOES 3 UNDERSHIRT, which wears the hat while it has no value of its own: PALSPLIT1), when the mod is on and that
   bro's choice is Custom; 0 otherwise. Asked whatever the session is: in a
   multiplayer run Custom stands down here (it would recolour every body wearing the file), and the room's picks
   (hal/bro_colors.cpp port_bro_my_row) start from this answer when the player has no online pick of his own for that bro, so the
   colours he chose in the mod are the ones he wears in a coop room by default. */
extern "C" int port_fs_mods_bro_custom(int chr, int part, unsigned *rgb)
{
    int row = 0;
    long v;
    if (chr < 0 || chr > 2 || part < 0 || part > 3)
        return 0;
    cp_mod_read();
    if (!g_cpm.on || modval::choice_kind(g_cpm.choice[chr], 0, &row) != modval::CP_CUSTOM)
        return 0;
    v = cp_bro_rgb(chr, part);
    if (v < 0)
        return 0;
    if (rgb)
        *rgb = (unsigned)v & 0xFFFFFFu;
    return 1;
}
