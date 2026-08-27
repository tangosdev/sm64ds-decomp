/* THE LEVEL-EDITS MOD: apply an external .lvlm file to a level's object
   tables at mount, so an editor outside the game can move, delete and add
   placed objects without the game's own code learning anything.

   Same rules as every other mod in this tree (hal/fs_mods.cpp states them for
   the palette bridge and they are not restated per mod): OFF IS THE ROM,
   boot-latched, and every disagreement refuses OUT LOUD and serves stock.
   The one addition here is that a refusal is WHOLE-FILE. A palette record that
   does not apply costs one texture its colors; a level-edit section that does
   not apply costs the level a sub-table it was going to replace, and the
   half-edited level that results is a shape nobody asked for and nobody can
   debug. So this file validates the entire file against the mounted level
   BEFORE it writes one word, and any single complaint drops all of it.

   ---- WHAT THE ROM'S LEVEL DATA ACTUALLY IS -------------------------------

   Read off the matched TUs, not off a description of them. The three that
   define the whole shape:

     src/_ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR12MeshCollider.cpp
         LVL_Overlay: clps +0x00, objTable +0x04, kclFileId +0x0a,
         subTables +0x10, subCount +0x14. It calls LoadObjects ONCE on
         ovl->objTable with index -1, then once per sub-table entry with
         index i:

             LoadObjects(ovl->objTable, -1, p);
             e = ovl->subTables;
             for (i = 0; i < ovl->subCount; i++) {
                 if (e->table != 0) LoadObjects(e->table, i, p);
                 e++;
             }

         LVL_SubTbl is { ObjTable* table; char pad[8]; } -- stride 0x0c, the
         pointer first. hal/level_boot.cpp's PortLvlOverlay says stride 0x0c
         independently and the two agree.

     src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.c
         ObjTable: u16 count +0x00, entries +0x04. The entries are 8 bytes
         each and they are DESCRIPTORS, not objects:

             b    = *e;
             type = (b >> 5) & 7;
             if (type == 0 || type == data_0209f220)
                 h = data_ov002_0210cbb8[b & 0x1f], h(e, a1, a2);
             e += 8;

         So descriptor byte 0 packs TWO fields: the low five bits pick the
         sub-loader, and the high three are a filter compared against
         data_0209f220.

     THE HIGH THREE BITS ARE THE STAR, NOT THE AREA, and this is worth saying
     plainly because it is the one thing an editor is most likely to get
     backwards. data_0209f220 is declared "current star filter" in
     hal/level_boot.cpp:2606 and "star filter" in hal/star_flow.cpp:75, it is
     written from data_0209f1f0 in hal/level_change.cpp:890, and
     hal/level_boot.cpp:2902 seeds it from SM64DS_STAR. A descriptor with 0
     there loads for every mission; one with 3 loads only while star 3 is
     selected. THE AREA IS THE OTHER AXIS ENTIRELY -- it is which ObjTable the
     descriptor lives in, the loop index the ROM passes down as LoadObjects'
     second argument, and that is what this file's `scope` byte selects.

     src/_Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij.cpp and its
     siblings
         ObjSubTable: kind +0x00 (the byte above), count +0x01, entries +0x04.
         The record shapes this build writes:

           type 0  standard  0x10  u16 raw, s16 x, s16 y, s16 z,
                                   s16 rot[3], u16 param
           type 1  entrance  0x10  the same sixteen bytes; LoadEntranceObjects
                                   reads param's low three bits as the area and
                                   bits 3..6 as the entrance id
           type 5  simple    0x08  u16 raw, s16 x, s16 y, s16 z, and raw is
                                   split: raw & 0x1ff indexes the actor table,
                                   raw >> 9 is the parameter

         `raw` is an index into data_ov002_0210cbf4[], the actor-id table --
         it is NOT the actor id. Both loaders spell that out.

   THE MAPPING THIS FILE'S `scope` BYTE MAKES, stated exactly because the
   format cannot be read without it:

       scope 0xFF  ->  ovl->objTable                     (the MISC table, the
                       one the ROM loads with index -1)
       scope 0..7  ->  *(ObjTable**)(ovl->subTables + scope*0x0c)
                       (the AREA table the ROM loads with index == scope)

   A scope past ovl->subCount is refused rather than clamped: the level does
   not have that area and an editor that thinks it does has a stale file.

   ---- WHY THE ARENA IS STATIC ---------------------------------------------

   Replacing a sub-table means pointing its `entries` word at a new array, and
   that word lives inside the mounted overlay image. The image is .dsstate --
   tools/ovdata.py routes every mounted byte into the captured section on
   purpose -- so anything this file writes there goes into save states.

   hal/lk6_savestate.cpp's cross-process scan is the constraint, and it is a
   check rather than a preference. lk6_savestate_scan_host_pointers walks every
   captured word and classifies its target: MEM_IMAGE and MEM_MAPPED pass, the
   pinned arena passes, a DS-address reservation passes, and anything else --
   "host heap or stack" -- is reported as DEAD in the next process. A malloc'd
   entry array is exactly that last case, so a save state taken after a
   level-edit load would carry a table pointer into freed memory and the
   restore would walk it.

   A file-scope array is in the exe's own image, so it comes back MEM_IMAGE and
   the scan stays at zero. That is the whole argument for the arena below.

   The bump pointer never rewinds. A save-state restore rolls the overlay image
   back to unpatched AND rolls hal/level_boot.cpp's g_level_mounted cache back
   with it (they are bracketed together, deliberately), so the next entry
   re-patches and this file re-applies and spends the arena again. That is
   bounded, not unbounded -- and when the arena does run out the section is
   refused out loud like any other disagreement, so the failure is a message
   rather than a corrupted table.

   ---- THE FILE FORMAT -----------------------------------------------------

   Little-endian throughout. port/tools/lvlmod.py is the reference writer and
   carries the same table in its docstring.

       "LVLM"  u8 version=1  u8 level_id  u16 section count
       then per section:
       u8 scope   u8 type   u16 count
       count * recordsize bytes, in the EXACT ROM record layout for that type

   A section REPLACES that (scope, type) sub-table's entry list wholesale. A
   sub-table the file does not name is left exactly as the ROM has it.

   ONE CAVEAT THIS FILE CANNOT CHECK, written down rather than guarded because
   the check is not available where the guard would have to go. Type 1 is the
   only loader that does not start at record zero:

       LoadEntranceObjects: e = tbl->entries; e += p3;
                            while (i < data_0209f21c) { ... e++; i++; }

   p3 is Stage::LoadClsnAndObjects' second argument and data_0209f21c is the
   player count, so the entrance loader reads records p3 .. p3 + players - 1.
   Neither value exists at mount time -- they are decided later, on the way
   into the level -- so a short entrance section cannot be refused here, and
   an editor that trims one below what the boot indexes will walk the loader
   off the end of its own array. Standard and simple sub-tables have no such
   rule: both loop from zero to count and a section of any length is safe. */

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/* LVL_Overlay, the fields this file reads. Offsets are hal/level_boot.cpp's
   PortLvlOverlay, which is the ROM's own shape; the port is a 32-bit build so
   a host pointer is the same four bytes the DS word was. */
struct LvlOverlay {
    u8 *clps;               /* 0x00 */
    u8 *objTable;           /* 0x04  ObjTable* -- the misc table */
    u16 bmdFileId;          /* 0x08 */
    u16 kclFileId;          /* 0x0a */
    u16 icgFileId;          /* 0x0c */
    u16 iclFileId;          /* 0x0e */
    u8 *subTables;          /* 0x10  LVL_SubTbl[], stride 0x0c */
    u8 subCount;            /* 0x14 */
    u8 flags;               /* 0x15 */
};

/* One 8-byte descriptor out of an ObjTable's entry array. */
struct ObjSubTable {
    u8 kind;                /* low 5 bits sub-loader, high 3 bits star filter */
    u8 count;               /* records in `entries` -- ONE BYTE, so max 255 */
    u8 pad[2];
    u8 *entries;
};

/* The table LoadObjects walks. Bytes 2..3 are not read by any matched TU that
   touches this, so they are left alone rather than named. */
struct ObjTable {
    u16 count;              /* number of ObjSubTable descriptors */
    u16 pad;
    ObjSubTable *entries;
};

const unsigned SCOPE_MISC = 0xFF;

/* Record size for a sub-loader index, 0 for one this build does not write.
   Refusing an unknown type is the point: the ROM has fifteen sub-loaders and
   this build has read the record shape off exactly three of them. Writing a
   guessed shape into a level is worse than not editing it. */
int record_size(unsigned type)
{
    switch (type) {
    case 0:  return 0x10;   /* LoadStandardObjects */
    case 1:  return 0x10;   /* LoadEntranceObjects */
    case 5:  return 0x08;   /* LoadSimpleObjects   */
    default: return 0;
    }
}

const char *type_name(unsigned type)
{
    switch (type) {
    case 0:  return "standard";
    case 1:  return "entrance";
    case 5:  return "simple";
    default: return "unknown";
    }
}

/* ---- the arena ---------------------------------------------------------- */

/* 256 KB. A sub-table cannot hold more than 255 records and the widest record
   is sixteen bytes, so one section is at most 4080 bytes and this is room for
   sixty-odd of them -- far past any level's sub-table count, with headroom for
   the re-applies a save-state restore causes. */
u8 g_arena[256 * 1024];
u32 g_arena_used;

u8 *arena_take(u32 n)
{
    u8 *p;
    if (n > sizeof g_arena - g_arena_used)
        return 0;
    p = g_arena + g_arena_used;
    g_arena_used += (n + 3u) & ~3u;     /* keep the next array word-aligned */
    return p;
}

/* ---- the file ----------------------------------------------------------- */

int g_state;            /* 0 not tried, 1 loaded, -1 off, absent or refused */
u8 *g_blob;
u32 g_blob_len;
int g_blob_level;
u32 g_nsection;
char g_shown[1024];

u16 rd16(const u8 *p, u32 at) { return (u16)(p[at] | (p[at + 1] << 8)); }
short rds16(const u8 *p, u32 at) { return (short)rd16(p, at); }

/* Every refusal in this file goes through here, so they all read the same way
   and they all say what happens instead. */
void refuse(const char *why)
{
    std::fprintf(stderr, "[mods] StageMod: %s; the level keeps its stock "
                         "objects\n", why);
    std::fflush(stderr);
}

void release(void)
{
    std::free(g_blob);
    g_blob = 0;
    g_blob_len = 0;
    g_nsection = 0;
}

/* Read and fully validate the file's own shape. Nothing here can look at the
   level -- the level is not mounted when this is called for the first mount --
   so this pass proves the file is self-consistent and the per-level checks
   happen in apply_sections below. */
void stage_mod_load(void)
{
    const char *path;
    std::FILE *f;
    long len;
    u32 cur, s;
    char msg[1280];

    if (g_state)
        return;
    g_state = -1;

    path = std::getenv("SM64DS_STAGE_MOD");
    if (!path || !*path)
        return;                 /* off, and off is silent like every mod's off */

    std::snprintf(g_shown, sizeof g_shown, "%s", path);
    f = std::fopen(path, "rb");
    if (!f) {
        std::snprintf(msg, sizeof msg,
                      "SM64DS_STAGE_MOD names %s and nothing there opens",
                      g_shown);
        refuse(msg);
        return;
    }
    std::fseek(f, 0, SEEK_END);
    len = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (len < 8 || len > 4 * 1024 * 1024) {
        std::fclose(f);
        std::snprintf(msg, sizeof msg,
                      "%s is %ld bytes, and a level-edits file is between 8 "
                      "and 4194304", g_shown, len);
        refuse(msg);
        return;
    }
    g_blob = (u8 *)std::malloc((size_t)len);
    if (!g_blob || (long)std::fread(g_blob, 1, (size_t)len, f) != len) {
        std::fclose(f);
        release();
        std::snprintf(msg, sizeof msg, "%s would not read", g_shown);
        refuse(msg);
        return;
    }
    std::fclose(f);
    g_blob_len = (u32)len;

    if (std::memcmp(g_blob, "LVLM", 4) != 0) {
        release();
        std::snprintf(msg, sizeof msg,
                      "%s does not start with LVLM, so it is not a "
                      "level-edits file", g_shown);
        refuse(msg);
        return;
    }
    if (g_blob[4] != 1) {
        /* THE MESSAGE IS BUILT BEFORE release(), and every refusal below does
           the same. release() frees the blob AND zeroes g_blob_len and
           g_nsection, so a message composed after it reads a freed pointer or
           reports zero. Both shapes shipped in the first draft of this file and
           both were caught by the refusal battery: the version arm faulted
           c0000005 reading g_blob[4] through the freed pointer, and the
           overrun arm printed "4294967284 bytes are left" from 0 - (cur + 4).
           A refusal path is the one path least likely to be exercised, so it
           is the one that has to be ordered right by construction. */
        std::snprintf(msg, sizeof msg,
                      "%s is level-edits version %u and this build reads "
                      "version 1", g_shown, g_blob[4]);
        release();
        refuse(msg);
        return;
    }
    g_blob_level = g_blob[5];
    g_nsection = rd16(g_blob, 6);

    /* Walk every section header and prove the whole file is accounted for. */
    cur = 8;
    for (s = 0; s < g_nsection; ++s) {
        unsigned scope, type, count;
        int rs;
        u32 need;

        if (cur + 4 > g_blob_len) {
            std::snprintf(msg, sizeof msg,
                          "%s says %u section(s) and section %u's header runs "
                          "past the end of the file", g_shown, g_nsection, s);
            release();
            refuse(msg);
            return;
        }
        scope = g_blob[cur];
        type = g_blob[cur + 1];
        count = rd16(g_blob, cur + 2);
        rs = record_size(type);
        if (!rs) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u names sub-table type %u, and this "
                          "build only writes 0 standard, 1 entrance and 5 "
                          "simple", g_shown, s, type);
            release();
            refuse(msg);
            return;
        }
        if (scope != SCOPE_MISC && scope > 7) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u names scope %u, and a scope is an "
                          "area 0..7 or 0xFF for the misc table",
                          g_shown, s, scope);
            release();
            refuse(msg);
            return;
        }
        if (count > 255) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u carries %u entries and a sub-table's "
                          "count is one byte, so 255 is the ceiling",
                          g_shown, s, count);
            release();
            refuse(msg);
            return;
        }
        need = count * (u32)rs;
        if (cur + 4 + need > g_blob_len) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u says %u %s entries (%u bytes) and "
                          "only %u bytes are left in the file",
                          g_shown, s, count, type_name(type), need,
                          g_blob_len - (cur + 4 <= g_blob_len
                                        ? cur + 4 : g_blob_len));
            release();
            refuse(msg);
            return;
        }
        cur += 4 + need;
    }
    if (cur != g_blob_len) {
        std::snprintf(msg, sizeof msg,
                      "%s has %u byte(s) left over after its last section, so "
                      "it is not the file it says it is",
                      g_shown, g_blob_len - cur);
        release();
        refuse(msg);
        return;
    }

    g_state = 1;
    std::fprintf(stderr, "[mods] StageMod: %u section(s) for level %d (%s)\n",
                 g_nsection, g_blob_level, g_shown);
    std::fflush(stderr);
}

/* ---- resolving a section onto the mounted level ------------------------- */

ObjTable *scope_table(LvlOverlay *ovl, unsigned scope)
{
    if (scope == SCOPE_MISC)
        return (ObjTable *)ovl->objTable;
    if (scope >= ovl->subCount)
        return 0;
    if (!ovl->subTables)
        return 0;
    return *(ObjTable **)(ovl->subTables + scope * 0x0c);
}

/* The one descriptor in `scope` whose sub-loader is `type`, or 0 with `why`
   set. Ambiguity is a refusal rather than a pick: a level that carries two
   standard sub-tables in one area carries them under different STAR filters,
   and a format whose section key is (scope, type) cannot say which of them the
   editor meant. Guessing would silently edit the wrong mission. */
ObjSubTable *find_descriptor(LvlOverlay *ovl, unsigned scope, unsigned type,
                             const char **why)
{
    ObjTable *t = scope_table(ovl, scope);
    ObjSubTable *hit = 0;
    unsigned i, n = 0;

    if (!t) {
        *why = (scope == SCOPE_MISC)
             ? "this level has no misc object table"
             : "this level has no such area";
        return 0;
    }
    if (!t->entries) {
        *why = "that table has no entry array";
        return 0;
    }
    for (i = 0; i < t->count; ++i) {
        ObjSubTable *d = &t->entries[i];
        if ((d->kind & 0x1f) == type) {
            hit = d;
            ++n;
        }
    }
    if (n == 0) {
        *why = "this level has no sub-table of that type there to replace";
        return 0;
    }
    if (n > 1) {
        *why = "this level has more than one sub-table of that type there "
               "(they differ by star filter) and a section cannot say which";
        return 0;
    }
    return hit;
}

/* TWO PASSES ON PURPOSE. The first resolves every section and reserves its
   arena bytes without writing one word into the level; only if all of them
   resolve does the second commit. That is what makes "never half-apply" a
   property of the code rather than a hope. */
void apply_sections(LvlOverlay *ovl, int level_id)
{
    struct Plan {
        ObjSubTable *desc;
        const u8 *src;
        u32 bytes;
        u16 count;
        u8 scope;
        u8 type;
    };
    Plan plan[64];
    u32 cur, s;
    u32 arena_mark = g_arena_used;
    char msg[1280];

    if (g_nsection > sizeof plan / sizeof plan[0]) {
        std::snprintf(msg, sizeof msg,
                      "%s carries %u sections and this build applies at most "
                      "%u", g_shown, g_nsection,
                      (unsigned)(sizeof plan / sizeof plan[0]));
        refuse(msg);
        return;
    }

    /* PASS ONE: resolve every section and reserve its arena bytes. Not one
       word of the level is written here, so a complaint at section five costs
       nothing that sections zero to four did. */
    cur = 8;
    for (s = 0; s < g_nsection; ++s) {
        unsigned scope = g_blob[cur];
        unsigned type = g_blob[cur + 1];
        unsigned count = rd16(g_blob, cur + 2);
        u32 need = count * (u32)record_size(type);
        const char *why = 0;
        ObjSubTable *d = find_descriptor(ovl, scope, type, &why);

        if (!d) {
            g_arena_used = arena_mark;
            if (scope == SCOPE_MISC)
                std::snprintf(msg, sizeof msg,
                              "%s section %u wants the misc %s sub-table and "
                              "%s", g_shown, s, type_name(type), why);
            else
                std::snprintf(msg, sizeof msg,
                              "%s section %u wants area %u's %s sub-table and "
                              "%s", g_shown, s, scope, type_name(type), why);
            refuse(msg);
            return;
        }
        if (!arena_take(need ? need : 4)) {
            g_arena_used = arena_mark;
            std::snprintf(msg, sizeof msg,
                          "%s section %u needs %u bytes and this build's "
                          "level-edit arena has %u left", g_shown, s, need,
                          (unsigned)(sizeof g_arena) - arena_mark);
            refuse(msg);
            return;
        }
        plan[s].desc = d;
        plan[s].src = g_blob + cur + 4;
        plan[s].bytes = need;
        plan[s].count = (u16)count;
        plan[s].scope = (u8)scope;
        plan[s].type = (u8)type;
        cur += 4 + need;
    }

    /* PASS TWO: commit. The arena is rewound to the mark and re-taken in the
       same order with the same sizes, so every section lands on the block pass
       one proved there was room for. */
    g_arena_used = arena_mark;
    for (s = 0; s < g_nsection; ++s) {
        u32 need = plan[s].bytes;
        u8 *dst = arena_take(need ? need : 4);

        if (need)
            std::memcpy(dst, plan[s].src, need);
        /* The kind byte is left exactly as the ROM wrote it: the sub-loader
           index and the star filter are the level's, not the editor's. Only
           the count and the array pointer move. */
        plan[s].desc->count = (u8)plan[s].count;
        plan[s].desc->entries = dst;

        if (plan[s].scope == SCOPE_MISC)
            std::fprintf(stderr, "[mods] StageMod: level %d misc %s sub-table "
                                 "replaced with %u entry(s)\n",
                         level_id, type_name(plan[s].type), plan[s].count);
        else
            std::fprintf(stderr, "[mods] StageMod: level %d area %u %s "
                                 "sub-table replaced with %u entry(s)\n",
                         level_id, plan[s].scope, type_name(plan[s].type),
                         plan[s].count);
    }
    std::fflush(stderr);
}

/* ---- the dump ----------------------------------------------------------- */

/* SM64DS_STAGE_MOD_DUMP=1 prints every sub-table of the mounted level and, for
   the three record shapes this build knows, every record in it -- read through
   the SAME pointer chain LoadObjects will walk, after any edit has been
   applied. That is the point: it describes what the game is about to load,
   not what a file said. Two runs of the same level diff cleanly, so it is also
   the proof instrument for an edit.

   No pointers are printed at level 1 for exactly that reason: an address would
   make every diff noisy. SM64DS_STAGE_MOD_DUMP=2 adds them for the times when
   the address is the question. */
int dump_level(void)
{
    static int v = -1;
    if (v < 0) {
        const char *e = std::getenv("SM64DS_STAGE_MOD_DUMP");
        v = e ? std::atoi(e) : 0;
    }
    return v;
}

void dump_record(unsigned type, const u8 *r)
{
    if (type == 0 || type == 1)
        std::fprintf(stderr, "raw 0x%04x pos %6d %6d %6d rot %6d %6d %6d "
                             "param 0x%04x\n",
                     rd16(r, 0), rds16(r, 2), rds16(r, 4), rds16(r, 6),
                     rds16(r, 8), rds16(r, 10), rds16(r, 12), rd16(r, 14));
    else
        std::fprintf(stderr, "raw 0x%04x pos %6d %6d %6d (id %u param %u)\n",
                     rd16(r, 0), rds16(r, 2), rds16(r, 4), rds16(r, 6),
                     rd16(r, 0) & 0x1ff, rd16(r, 0) >> 9);
}

void dump_table(int level_id, unsigned scope, ObjTable *t)
{
    char where[32];
    unsigned i, j;

    if (scope == SCOPE_MISC)
        std::snprintf(where, sizeof where, "misc");
    else
        std::snprintf(where, sizeof where, "area %u", scope);

    if (!t) {
        std::fprintf(stderr, "[lvldump] level %d %s: no table\n",
                     level_id, where);
        return;
    }
    std::fprintf(stderr, "[lvldump] level %d %s: %u sub-table(s)\n",
                 level_id, where, t->count);
    if (!t->entries)
        return;
    for (i = 0; i < t->count; ++i) {
        ObjSubTable *d = &t->entries[i];
        unsigned type = d->kind & 0x1f;
        unsigned star = d->kind >> 5;
        int rs = record_size(type);

        std::fprintf(stderr, "[lvldump] level %d %s sub %u: type %2u %-8s "
                             "star %u count %3u",
                     level_id, where, i, type, type_name(type), star,
                     d->count);
        if (dump_level() >= 2)
            std::fprintf(stderr, " entries %p", (void *)d->entries);
        std::fprintf(stderr, "\n");
        if (!rs || !d->entries)
            continue;
        for (j = 0; j < d->count; ++j) {
            std::fprintf(stderr, "[lvldump] level %d %s sub %u rec %3u: ",
                         level_id, where, i, j);
            dump_record(type, d->entries + (size_t)j * rs);
        }
    }
}

void dump_level_tables(int level_id, LvlOverlay *ovl)
{
    unsigned a;
    if (!dump_level())
        return;
    std::fprintf(stderr, "[lvldump] level %d: subCount %u flags 0x%02x\n",
                 level_id, ovl->subCount, ovl->flags);
    dump_table(level_id, SCOPE_MISC, (ObjTable *)ovl->objTable);
    for (a = 0; a < ovl->subCount; ++a)
        dump_table(level_id, a, scope_table(ovl, a));
    std::fflush(stderr);
}

}  /* namespace */

/* THE ONE ENTRY POINT. hal/level_boot.cpp calls this from port_level_mount_at
   after d->patch() and after d->at() has resolved the LVL_Overlay, which is
   the only window that is both late enough for the table pointers to be host
   pointers and early enough that nothing has read them: Stage::InitResources
   reaches Stage::LoadClsnAndObjects later, in matched src. */
extern "C" void port_stage_mod_apply(int level_id, void *lvlv)
{
    LvlOverlay *ovl = (LvlOverlay *)lvlv;

    if (!ovl)
        return;
    stage_mod_load();
    if (g_state == 1) {
        if (g_blob_level != level_id) {
            char msg[1280];
            std::snprintf(msg, sizeof msg,
                          "%s is for level %d and this boot mounted level %d",
                          g_shown, g_blob_level, level_id);
            refuse(msg);
        } else {
            apply_sections(ovl, level_id);
        }
    }
    dump_level_tables(level_id, ovl);
}
