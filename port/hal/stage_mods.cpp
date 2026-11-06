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

     src/_ZN5Stage18LoadClsnAndObjectsER11LVL_OverlayjR7dBgW_Kc.cpp
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

     src/_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij.cpp
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
       then per section, SIX bytes of header:
       u8 scope   u8 type   u8 star   u8 reserved   u16 count
       count * recordsize bytes, in the EXACT ROM record layout for that type

   A section REPLACES the (scope, star, type) sub-table's entry list wholesale.
   A sub-table the file does not name is left exactly as the ROM has it.

   THE STAR IS PART OF THE KEY BECAUSE THE ROM MAKES IT PART OF THE KEY, and
   this was measured before it was written. A level gives each mission its own
   object list, so one area routinely carries several sub-tables of the same
   type differing only in the star filter: Bob-omb Battlefield's area 0 holds
   eight standard sub-tables and eight simple ones, stars 0 through 7. Over the
   forty-six levels this build mounts there are 366 sub-tables of an editable
   type; a (scope, type) key names only 147 of them and leaves 219 unreachable,
   while (scope, star, type) names all 366 with no collisions. So the section
   key is the whole of the ROM's own descriptor byte, split across two fields
   for readability, and find_descriptor below matches that byte exactly.

   The reserved byte must be zero. It is the format's only room to grow and a
   reader that ignored it would silently mis-load a future file.

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
   rule: both loop from zero to count and a section of any length is safe.

   ---- SECTION TYPE 0xF0: STAGE PROPERTIES ---------------------------------

   A properties section does not name a sub-table at all. It is scope 0xFF,
   type 0xF0, star 0, reserved 0, a u16 count, then count FOUR-byte records:

       u8 field   u8 value   u16 pad (must be 0)

   Field 0 is the skybox. Value 0 means no skybox and 1..11 are the ROM's own
   sky ids. An unknown FIELD id refuses the whole file, which is the same
   never-half-apply law the rest of this file runs on and is the reason the
   field space can grow later without a new section type: a build that predates
   a field says so and serves stock rather than applying the half it knows.

   WHERE THE SKYBOX ACTUALLY LIVES, derived off the matched TUs and then
   checked against the ROM's own bytes, because the format note this was
   written from said "+0x78" and that is an offset in the level header FILE,
   not in the record the game reads.

     src/_ZN5Stage11GetSkyboxIDEv.cpp is the whole of the read:

         extern int data_0209f340[];
         int _ZN5Stage11GetSkyboxIDEv(void){
           unsigned int *p=*(unsigned int**)data_0209f340;
           return (p[6] >> 4) & 0x1f;
         }

     data_0209f340 holds the CURRENT LVL_Overlay pointer -- hal/level_boot.cpp
     :3075 sets it to the very pointer this file is handed, and the ROM sets it
     from data_02092208[level] at src/_ZN5Stage13InitResourcesEv.cpp:360. So
     p is the LVL_Overlay and p[6] is the WORD AT +0x18. The skybox is bits
     4..8 of that word.

     THE FIELD IS FIVE BITS AND IT CROSSES A BYTE BOUNDARY. Bit 8 is the low
     bit of the byte at +0x19, so this is a read-modify-write of the whole
     32-bit word and not a byte poke. Writing one byte would leave bit 8 as
     the level had it and turn a requested id 3 into 19.

     Checked against extracted/arm9_dec.bin and the overlay images rather than
     believed: ov009's LVL_Overlay (0x02112bdc, level 1) and ov014's
     (0x02113434, level 6) both carry 0x0000001f at +0x18, which is skybox 1 --
     and port/tests/walk_window.cpp:6152 independently says the castle grounds
     sky is id 1. The two agree.

   WHY 1..11 IS THE CEILING. Stage::LoadSkybox indexes data_02075620 at
   skyboxID - 1 (src/_ZN5Stage10LoadSkyboxEv.cpp:26). That table runs to the next
   symbol at 0x02075638, so it is twelve u16 slots, and the ROM's bytes are
   2040..2050 in the first eleven and ZERO in the twelfth. Eleven real vrbox
   handles, then a slot that would hand Model::LoadAndSetFile file 0. The ROM's
   own mask is 0x1f, so the hardware would carry 12..31 straight off the end of
   that table; this file refuses them instead.

   WHEN IT LANDS. Stage::InitResources runs its loaders in one order
   (src/_ZN5Stage13InitResourcesEv.cpp): :361 LoadModel, :363
   LoadClsnAndObjects, :388 LoadTextureTransformers, :389 LoadSkybox. The
   object tables this file already rewrites are read at :363 and the skybox id
   is read at :389, so a skybox patch made in the same mount window as the
   object patch is not merely early enough, it is LATER-consumed than the edit
   this file has always made. */

#include <cstdio>
#include <cstdlib>
#include <cstring>

/* hal/mods.cpp's port_mods_stage_entry, through a pointer defined at the foot
   of this file, because the narrow smoke targets link this file without
   hal/mods.cpp. */
extern "C" int (*port_stage_entry_hook)(int i, char *path, unsigned cap,
                                        const char **mod);

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
    u8 pad16[2];            /* 0x16 */
    /* 0x18. Stage::GetSkyboxID reads this as p[6] and takes bits 4..8; the
       rest of the word belongs to the level and is never written here.
       hal/level_boot.cpp's PortLvlOverlay carries the same word as unk18. */
    u32 settings;
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

/* The properties section. Not a sub-loader index: 0xF0 is outside the ROM's
   five-bit sub-table kind entirely, which is what lets it share the section
   header without ever colliding with a real sub-table type. */
const unsigned TYPE_PROPS = 0xF0;
const int PROPS_RECSIZE = 4;

/* Property field ids. One so far; the space grows here rather than by adding
   section types. */
const unsigned FIELD_SKYBOX = 0;
const unsigned SKYBOX_MAX = 11;     /* data_02075620 holds eleven vrbox handles */

/* Where the skybox sits inside the LVL_Overlay's settings word. */
const u32 SKYBOX_SHIFT = 4;
const u32 SKYBOX_MASK = 0x1Fu << SKYBOX_SHIFT;

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

/* The size of one record in a section of `type`, properties included. Every
   bounds check in the loader goes through this so a properties section is
   accounted for in the file walk exactly like any other. */
int section_recsize(unsigned type)
{
    if (type == TYPE_PROPS)
        return PROPS_RECSIZE;
    return record_size(type);
}

const char *type_name(unsigned type)
{
    switch (type) {
    case 0:  return "standard";
    case 1:  return "entrance";
    case 5:  return "simple";
    case TYPE_PROPS: return "properties";
    default: return "unknown";
    }
}

const char *field_name(unsigned field)
{
    switch (field) {
    case FIELD_SKYBOX: return "skybox";
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

/* One level-edits file in memory, validated against itself. Two of them live
   for the whole run: the one SM64DS_STAGE_MOD names (read once, at the first
   mount, as it always was) and the one a mod's stage/ supplies for the level
   being mounted now (read at that mount, the previous one released first). */
struct Lvlm {
    u8 *blob;
    u32 len;
    int level;
    u32 nsection;
    char shown[1024];
};

int g_env_state;        /* 0 not tried, 1 loaded, -1 off, absent or refused */
Lvlm g_env;
Lvlm g_mod;
/* Which stage-property fields the file has already set, so a field named twice
   anywhere in it is refused rather than resolved by ordering. Indexed by the
   record's field byte, so it covers the whole 0..255 space a file can spell. */
u8 g_seen_field[256];

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

void release(Lvlm &L)
{
    std::free(L.blob);
    L.blob = 0;
    L.len = 0;
    L.nsection = 0;
}

/* Read and fully validate the file's own shape into L (whatever L held is
   released first). Nothing here can look at the level (the level is not
   mounted when this is called for the first mount), so this pass proves the
   file is self-consistent and the per-level checks happen in apply_sections
   below. Returns 1 loaded, 0 refused out loud (L then holds no file). `mod` is
   the mod's id for a file out of a mod's stage/ folder and 0 for the file
   SM64DS_STAGE_MOD names; `quiet` leaves out the one success line, for the
   boot index below, which reads every mod file once only to learn its level. */
int load_file(Lvlm &L, const char *path, const char *mod, int quiet)
{
    std::FILE *f;
    long len;
    u32 cur, s;
    char msg[1280];

    release(L);
    std::snprintf(L.shown, sizeof L.shown, "%s", path);
    f = std::fopen(path, "rb");
    if (!f) {
        if (mod)
            std::snprintf(msg, sizeof msg,
                          "%s is in mod %s's stage/ folder and will not open",
                          L.shown, mod);
        else
            std::snprintf(msg, sizeof msg,
                          "SM64DS_STAGE_MOD names %s and nothing there opens",
                          L.shown);
        refuse(msg);
        return 0;
    }
    std::fseek(f, 0, SEEK_END);
    len = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (len < 8 || len > 4 * 1024 * 1024) {
        std::fclose(f);
        std::snprintf(msg, sizeof msg,
                      "%s is %ld bytes, and a level-edits file is between 8 "
                      "and 4194304", L.shown, len);
        refuse(msg);
        return 0;
    }
    L.blob = (u8 *)std::malloc((size_t)len);
    if (!L.blob || (long)std::fread(L.blob, 1, (size_t)len, f) != len) {
        std::fclose(f);
        release(L);
        std::snprintf(msg, sizeof msg, "%s would not read", L.shown);
        refuse(msg);
        return 0;
    }
    std::fclose(f);
    L.len = (u32)len;

    if (std::memcmp(L.blob, "LVLM", 4) != 0) {
        release(L);
        std::snprintf(msg, sizeof msg,
                      "%s does not start with LVLM, so it is not a "
                      "level-edits file", L.shown);
        refuse(msg);
        return 0;
    }
    if (L.blob[4] != 1) {
        /* THE MESSAGE IS BUILT BEFORE release(), and every refusal below does
           the same. release() frees the blob AND zeroes L.len and
           L.nsection, so a message composed after it reads a freed pointer or
           reports zero. Both shapes shipped in the first draft of this file and
           both were caught by the refusal battery: the version arm faulted
           c0000005 reading L.blob[4] through the freed pointer, and the
           overrun arm printed "4294967284 bytes are left" from 0 - (cur + 4).
           A refusal path is the one path least likely to be exercised, so it
           is the one that has to be ordered right by construction. */
        std::snprintf(msg, sizeof msg,
                      "%s is level-edits version %u and this build reads "
                      "version 1", L.shown, L.blob[4]);
        release(L);
        refuse(msg);
        return 0;
    }
    L.level = L.blob[5];
    L.nsection = rd16(L.blob, 6);

    /* Walk every section header and prove the whole file is accounted for. */
    std::memset(g_seen_field, 0, sizeof g_seen_field);
    cur = 8;
    for (s = 0; s < L.nsection; ++s) {
        unsigned scope, type, star, reserved, count;
        int rs;
        u32 need;

        if (cur + 6 > L.len) {
            std::snprintf(msg, sizeof msg,
                          "%s says %u section(s) and section %u's header runs "
                          "past the end of the file", L.shown, L.nsection, s);
            release(L);
            refuse(msg);
            return 0;
        }
        scope = L.blob[cur];
        type = L.blob[cur + 1];
        star = L.blob[cur + 2];
        reserved = L.blob[cur + 3];
        count = rd16(L.blob, cur + 4);
        rs = section_recsize(type);
        if (!rs) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u names sub-table type %u, and this "
                          "build only writes 0 standard, 1 entrance, 5 simple "
                          "and 240 stage properties", L.shown, s, type);
            release(L);
            refuse(msg);
            return 0;
        }
        /* A properties section is addressed to the level rather than to one of
           its sub-tables, so the two key bytes that name a sub-table have
           exactly one legal spelling. Refusing anything else keeps a future
           reader from having to guess what an editor meant by "area 3's
           properties", which is not a thing. */
        if (type == TYPE_PROPS && scope != SCOPE_MISC) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u is a stage-properties section with "
                          "scope %u, and stage properties belong to the level, "
                          "so their scope is 255", L.shown, s, scope);
            release(L);
            refuse(msg);
            return 0;
        }
        if (type == TYPE_PROPS && star != 0) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u is a stage-properties section with "
                          "star %u, and stage properties are not per-mission, "
                          "so their star is 0", L.shown, s, star);
            release(L);
            refuse(msg);
            return 0;
        }
        if (scope != SCOPE_MISC && scope > 7) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u names scope %u, and a scope is an "
                          "area 0..7 or 0xFF for the misc table",
                          L.shown, s, scope);
            release(L);
            refuse(msg);
            return 0;
        }
        if (star > 7) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u names star %u, and a star filter is "
                          "0..7 -- it is the top three bits of the ROM's own "
                          "sub-table byte", L.shown, s, star);
            release(L);
            refuse(msg);
            return 0;
        }
        /* The reserved byte is refused rather than ignored ON PURPOSE. It is
           the format's only room to grow, and a reader that skips it now would
           happily load a future file whose extra field it does not implement --
           silently doing the wrong thing with someone's level. Refusing costs
           nothing today and is the whole value of the byte. */
        if (reserved != 0) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u has %u in its reserved byte and this "
                          "build only reads 0 there", L.shown, s, reserved);
            release(L);
            refuse(msg);
            return 0;
        }
        if (count > 255) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u carries %u entries and a sub-table's "
                          "count is one byte, so 255 is the ceiling",
                          L.shown, s, count);
            release(L);
            refuse(msg);
            return 0;
        }
        need = count * (u32)rs;
        if (cur + 6 + need > L.len) {
            std::snprintf(msg, sizeof msg,
                          "%s section %u says %u %s entries (%u bytes) and "
                          "only %u bytes are left in the file",
                          L.shown, s, count, type_name(type), need,
                          L.len - (cur + 6 <= L.len
                                        ? cur + 6 : L.len));
            release(L);
            refuse(msg);
            return 0;
        }
        /* PROPERTY RECORDS ARE VALIDATED HERE, in the file pass, because
           nothing about them depends on the level: a field id and a value are
           either ones this build knows or they are not. Doing it here means an
           editor's mistake is refused before a level is even mounted, and it
           means apply_sections cannot fail on a properties section at all. */
        if (type == TYPE_PROPS) {
            unsigned r;
            for (r = 0; r < count; ++r) {
                const u8 *rec = L.blob + cur + 6 + r * PROPS_RECSIZE;
                unsigned field = rec[0];
                unsigned value = rec[1];
                unsigned pad = rd16(rec, 2);

                if (pad != 0) {
                    std::snprintf(msg, sizeof msg,
                                  "%s section %u property %u has %u in its "
                                  "padding and this build only reads 0 there",
                                  L.shown, s, r, pad);
                    release(L);
                    refuse(msg);
                    return 0;
                }
                /* THE UNKNOWN FIELD IS THE WHOLE POINT OF THE RULE. A build
                   that skipped a field it did not implement would apply the
                   half of someone's level it understood and silently drop the
                   rest, which is the one outcome this file exists to prevent.
                   Refusing means a newer file meets an older build with a
                   message instead of a mystery. */
                if (field != FIELD_SKYBOX) {
                    std::snprintf(msg, sizeof msg,
                                  "%s section %u property %u sets stage field "
                                  "%u and this build only knows field 0, the "
                                  "skybox", L.shown, s, r, field);
                    release(L);
                    refuse(msg);
                    return 0;
                }
                if (value > SKYBOX_MAX) {
                    std::snprintf(msg, sizeof msg,
                                  "%s section %u asks for skybox %u, and the "
                                  "game has 0 for none and 1 to %u for the "
                                  "vrbox skies", L.shown, s, value,
                                  SKYBOX_MAX);
                    release(L);
                    refuse(msg);
                    return 0;
                }
                /* One field, one answer. A scalar property named twice is a
                   file that does not know what it wants, and quietly taking
                   the last record would make which one won depend on the
                   writer's ordering. */
                if (g_seen_field[field]) {
                    std::snprintf(msg, sizeof msg,
                                  "%s sets the %s more than once, and a stage "
                                  "property has one value", L.shown,
                                  field_name(field));
                    release(L);
                    refuse(msg);
                    return 0;
                }
                g_seen_field[field] = 1;
            }
        }
        cur += 6 + need;
    }
    if (cur != L.len) {
        std::snprintf(msg, sizeof msg,
                      "%s has %u byte(s) left over after its last section, so "
                      "it is not the file it says it is",
                      L.shown, L.len - cur);
        release(L);
        refuse(msg);
        return 0;
    }

    if (!quiet) {
        std::fprintf(stderr, "[mods] StageMod: %u section(s) for level %d (%s)\n",
                     L.nsection, L.level, L.shown);
        std::fflush(stderr);
    }
    return 1;
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

/* The one descriptor in `scope` whose ROM byte is `star`:`type`, or 0 with
   `why` set.

   THE MATCH IS ON THE WHOLE BYTE, which is the ROM's own key for a sub-table:
   the sub-loader in the low five bits and the star filter in the high three.
   Nothing is masked off and nothing is inferred, so a section names exactly
   what the level data names.

   The duplicate check below is kept even though the whole byte is matched now.
   Over the forty-six mounted levels no two descriptors in one table share a
   byte -- measured, not assumed -- so it cannot fire on stock data. It stays
   because the alternative to refusing an impossible case is silently taking
   the last one, and editing a different sub-table than the one named is the
   worst failure this file could have. */
ObjSubTable *find_descriptor(LvlOverlay *ovl, unsigned scope, unsigned star,
                             unsigned type, const char **why)
{
    ObjTable *t = scope_table(ovl, scope);
    const u8 want = (u8)((star << 5) | type);
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
        if (d->kind == want) {
            hit = d;
            ++n;
        }
    }
    if (n == 0) {
        *why = "this level has no sub-table with that star and type there to "
               "replace";
        return 0;
    }
    if (n > 1) {
        *why = "this level carries more than one sub-table with that exact "
               "star and type, which no stock level does";
        return 0;
    }
    return hit;
}

/* TWO PASSES ON PURPOSE. The first resolves every section and reserves its
   arena bytes without writing one word into the level; only if all of them
   resolve does the second commit. That is what makes "never half-apply" a
   property of the code rather than a hope. */
void apply_sections(Lvlm &L, LvlOverlay *ovl, int level_id)
{
    struct Plan {
        ObjSubTable *desc;
        const u8 *src;
        u32 bytes;
        u16 count;
        u8 scope;
        u8 star;
        u8 type;
    };
    Plan plan[64];
    u32 cur, s;
    u32 arena_mark = g_arena_used;
    char msg[1280];

    if (L.nsection > sizeof plan / sizeof plan[0]) {
        std::snprintf(msg, sizeof msg,
                      "%s carries %u sections and this build applies at most "
                      "%u", L.shown, L.nsection,
                      (unsigned)(sizeof plan / sizeof plan[0]));
        refuse(msg);
        return;
    }

    /* PASS ONE: resolve every section and reserve its arena bytes. Not one
       word of the level is written here, so a complaint at section five costs
       nothing that sections zero to four did. */
    cur = 8;
    for (s = 0; s < L.nsection; ++s) {
        unsigned scope = L.blob[cur];
        unsigned type = L.blob[cur + 1];
        unsigned star = L.blob[cur + 2];
        unsigned count = rd16(L.blob, cur + 4);
        u32 need = count * (u32)section_recsize(type);
        const char *why = 0;
        ObjSubTable *d;

        /* A properties section resolves against nothing: it names no
           sub-table, spends no arena, and every record in it was already
           proved legal by the file pass. So it cannot fail here, which is
           what lets it ride the same two-pass commit without weakening the
           all-or-nothing guarantee. */
        if (type == TYPE_PROPS) {
            plan[s].desc = 0;
            plan[s].src = L.blob + cur + 6;
            plan[s].bytes = need;
            plan[s].count = (u16)count;
            plan[s].scope = (u8)scope;
            plan[s].star = (u8)star;
            plan[s].type = (u8)type;
            cur += 6 + need;
            continue;
        }

        d = find_descriptor(ovl, scope, star, type, &why);

        if (!d) {
            g_arena_used = arena_mark;
            if (scope == SCOPE_MISC)
                std::snprintf(msg, sizeof msg,
                              "%s section %u wants the misc star %u %s "
                              "sub-table and %s",
                              L.shown, s, star, type_name(type), why);
            else
                std::snprintf(msg, sizeof msg,
                              "%s section %u wants area %u's star %u %s "
                              "sub-table and %s",
                              L.shown, s, scope, star, type_name(type), why);
            refuse(msg);
            return;
        }
        if (!arena_take(need ? need : 4)) {
            g_arena_used = arena_mark;
            std::snprintf(msg, sizeof msg,
                          "%s section %u needs %u bytes and this build's "
                          "level-edit arena has %u left", L.shown, s, need,
                          (unsigned)(sizeof g_arena) - arena_mark);
            refuse(msg);
            return;
        }
        plan[s].desc = d;
        plan[s].src = L.blob + cur + 6;
        plan[s].bytes = need;
        plan[s].count = (u16)count;
        plan[s].scope = (u8)scope;
        plan[s].star = (u8)star;
        plan[s].type = (u8)type;
        cur += 6 + need;
    }

    /* PASS TWO: commit. The arena is rewound to the mark and re-taken in the
       same order with the same sizes, so every section lands on the block pass
       one proved there was room for. */
    g_arena_used = arena_mark;
    for (s = 0; s < L.nsection; ++s) {
        u32 need = plan[s].bytes;
        u8 *dst;

        if (plan[s].type == TYPE_PROPS) {
            unsigned r;
            for (r = 0; r < plan[s].count; ++r) {
                const u8 *rec = plan[s].src + r * PROPS_RECSIZE;
                unsigned value = rec[1];
                unsigned was;

                /* rec[0] is FIELD_SKYBOX: the file pass refused every other
                   field id, so there is nothing else this loop can be asked
                   to write. */
                was = (ovl->settings & SKYBOX_MASK) >> SKYBOX_SHIFT;
                /* READ-MODIFY-WRITE OF THE WORD, not a byte poke. The field is
                   five bits at 4..8, so it straddles the +0x18/+0x19 boundary
                   and a byte write would leave bit 8 as the level had it. The
                   other twenty-seven bits are the level's and are preserved
                   exactly. */
                ovl->settings = (ovl->settings & ~SKYBOX_MASK)
                              | ((u32)value << SKYBOX_SHIFT);
                std::fprintf(stderr, "[mods] StageMod: level %d skybox %u -> "
                                     "%u%s\n", level_id, was, value,
                             value ? "" : " (none)");
            }
            continue;
        }

        dst = arena_take(need ? need : 4);
        if (need)
            std::memcpy(dst, plan[s].src, need);
        /* The kind byte is left exactly as the ROM wrote it: the sub-loader
           index and the star filter are the level's, not the editor's. Only
           the count and the array pointer move. */
        plan[s].desc->count = (u8)plan[s].count;
        plan[s].desc->entries = dst;

        if (plan[s].scope == SCOPE_MISC)
            std::fprintf(stderr, "[mods] StageMod: level %d misc star %u %s "
                                 "sub-table replaced with %u entry(s)\n",
                         level_id, plan[s].star, type_name(plan[s].type),
                         plan[s].count);
        else
            std::fprintf(stderr, "[mods] StageMod: level %d area %u star %u %s "
                                 "sub-table replaced with %u entry(s)\n",
                         level_id, plan[s].scope, plan[s].star,
                         type_name(plan[s].type), plan[s].count);
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
    /* The stage properties, read back through the SAME word Stage::GetSkyboxID
       will read, after any edit has been applied. That is what makes it a
       proof instrument as well as the way an editor learns the stock value:
       the number printed here is the number the game is about to use. The
       whole settings word rides along at dump level 2 because the other
       twenty-seven bits are the level's and a properties edit must be seen not
       to have moved them. */
    std::fprintf(stderr, "[lvldump] level %d properties: skybox %u\n",
                 level_id,
                 (unsigned)((ovl->settings & SKYBOX_MASK) >> SKYBOX_SHIFT));
    if (dump_level() >= 2)
        std::fprintf(stderr, "[lvldump] level %d settings word: 0x%08x\n",
                     level_id, (unsigned)ovl->settings);
    dump_table(level_id, SCOPE_MISC, (ObjTable *)ovl->objTable);
    for (a = 0; a < ovl->subCount; ++a)
        dump_table(level_id, a, scope_table(ovl, a));
    std::fflush(stderr);
}

/* ---- the file SM64DS_STAGE_MOD names ------------------------------------ */

/* Read once, at the first mount, exactly as before mods could carry level
   edits: the Studio's own launch names its export here and keeps working
   unchanged. */
void env_load(void)
{
    const char *path;

    if (g_env_state)
        return;
    g_env_state = -1;

    path = std::getenv("SM64DS_STAGE_MOD");
    if (!path || !*path)
        return;                 /* off, and off is silent like every mod's off */
    if (load_file(g_env, path, 0, 0))
        g_env_state = 1;
}

/* ---- level edits out of the mods (run studio1, lane PORTLVLM1) ----------

   A mod's stage/ folder may hold any number of <anything>.lvlm files
   (hal/mods.cpp lists them, latest mod first). The LEVEL a file is for is
   byte 5 of its own header, never its name, so the Studio can copy its
   export folder in as it is.

   THE INDEX is built once, at the first mount: every listed file is read and
   validated whole by load_file, the same pass the SM64DS_STAGE_MOD file gets,
   and kept only for its level number. A file that fails is refused out loud
   right there and claims no level, so the levels it might have meant stay
   stock. Then, per level:

     - the first file met wins, which is the LATER mod in EnabledMods, the
       rule files/ and textures_hd/ use in hal/mods.cpp; every file it beats is
       named in a line;
     - two files for the SAME level inside ONE mod is a mod that does not know
       what it wants: both are refused in a line and the level is stock. An
       earlier mod's file for that level does not step in, because the mod that
       outranks it asked for something and the answer to a disagreement is
       always stock, never a guess.

   THE FILE ITSELF is read again at the mount of its level (g_mod; the file
   held before is released first) and applied by the same apply_sections. A
   second mount of the same level, a save-state restore included (the restore
   rolls the overlay and hal/level_boot.cpp's g_level_mounted back together, so
   the next entry re-patches and calls this again), re-applies the bytes
   already held, the same way the SM64DS_STAGE_MOD file is re-applied.

   SM64DS_STAGE_MOD STILL WINS, for the level its file is for. Every other
   level takes what the mods give it. A refused SM64DS_STAGE_MOD file names no
   level, so it outranks nothing.

   DECISION (lane PORTLVLM1): the index is a few hundred bytes per file and a
   map project's export is a few kilobytes, so the boot read is cheap; the
   alternative of reading headers only would let a broken file claim a level
   and then fail at its mount, which is the later and worse place to learn it. */
enum { MAX_MOD_LVLM = 256, LEVEL_COUNT = 256 };

struct ModLvlm {
    char path[1024];
    char mod[96];
    int level;
};

ModLvlm g_found[MAX_MOD_LVLM];
unsigned g_nfound;
int g_pick[LEVEL_COUNT];        /* index into g_found, or -1 when no mod names it */
u8 g_dup[LEVEL_COUNT];          /* the winning mod carries two files for it */
u8 g_said_outranked[LEVEL_COUNT];
int g_indexed;

void index_mods(void)
{
    static char path[1024];
    static Lvlm probe;
    const char *mod;
    int i, kind;
    char msg[2600];

    if (g_indexed)
        return;
    g_indexed = 1;
    for (i = 0; i < LEVEL_COUNT; ++i)
        g_pick[i] = -1;
    if (!port_stage_entry_hook)
        return;                 /* a target without the mods layer */

    for (i = 0; (kind = port_stage_entry_hook(i, path, sizeof path, &mod)) != 0;
         ++i) {
        int lv, p;
        ModLvlm *e;

        if (kind != 1)
            continue;           /* a _geom folder: hal/stage_geom.cpp's */
        if (!mod)
            mod = "?";
        if (!load_file(probe, path, mod, 1))
            continue;           /* refused out loud already, claims no level */
        lv = probe.level;
        release(probe);
        if (g_nfound >= MAX_MOD_LVLM) {
            std::snprintf(msg, sizeof msg,
                          "the enabled mods carry more than %u level-edits "
                          "files, so %s (mod %s) and every one after it are "
                          "not read", (unsigned)MAX_MOD_LVLM, path, mod);
            refuse(msg);
            break;
        }
        e = &g_found[g_nfound];
        std::snprintf(e->path, sizeof e->path, "%s", path);
        std::snprintf(e->mod, sizeof e->mod, "%s", mod);
        e->level = lv;
        p = g_pick[lv];
        if (p < 0) {
            g_pick[lv] = (int)g_nfound;
        } else if (std::strcmp(g_found[p].mod, mod) == 0) {
            std::snprintf(msg, sizeof msg,
                          "mod %s carries more than one level-edits file for "
                          "level %d (%s and %s), so none of them applies",
                          mod, lv, g_found[p].path, path);
            g_dup[lv] = 1;
            refuse(msg);
        } else {
            std::fprintf(stderr, "[mods] StageMod: level %d: %s (mod %s) is "
                                 "outranked by mod %s, later in EnabledMods%s\n",
                         lv, path, mod, g_found[p].mod,
                         g_dup[lv] ? ", whose own files for it were refused"
                                   : "");
        }
        ++g_nfound;
    }
    for (i = 0; i < LEVEL_COUNT; ++i)
        if (g_pick[i] >= 0 && !g_dup[i])
            std::fprintf(stderr, "[mods] StageMod: level %d takes %s (mod %s) "
                                 "at its mount\n", i, g_found[g_pick[i]].path,
                         g_found[g_pick[i]].mod);
    std::fflush(stderr);
}

}  /* namespace */

/* Set by hal/mods.cpp when it is linked (the two game targets): its
   port_mods_stage_entry. Null in the narrow targets, which have no mods. */
extern "C" int (*port_stage_entry_hook)(int i, char *path, unsigned cap,
                                        const char **mod) = 0;

/* THE ONE ENTRY POINT. hal/level_boot.cpp calls this from port_level_mount_at
   after d->patch() and after d->at() has resolved the LVL_Overlay, which is
   the only window that is both late enough for the table pointers to be host
   pointers and early enough that nothing has read them: Stage::InitResources
   reaches Stage::LoadClsnAndObjects later, in matched src. */
extern "C" void port_stage_mod_apply(int level_id, void *lvlv)
{
    LvlOverlay *ovl = (LvlOverlay *)lvlv;
    int pick;

    if (!ovl)
        return;
    env_load();
    index_mods();
    pick = (level_id >= 0 && level_id < LEVEL_COUNT && !g_dup[level_id])
         ? g_pick[level_id] : -1;

    if (g_env_state == 1 && g_env.level == level_id) {
        if (pick >= 0 && !g_said_outranked[level_id]) {
            g_said_outranked[level_id] = 1;
            std::fprintf(stderr, "[mods] StageMod: level %d: %s (mod %s) is "
                                 "outranked by SM64DS_STAGE_MOD (%s)\n",
                         level_id, g_found[pick].path, g_found[pick].mod,
                         g_env.shown);
        }
        apply_sections(g_env, ovl, level_id);
    } else if (pick >= 0) {
        const ModLvlm *e = &g_found[pick];
        /* Held already (a second entry, or the re-apply after a save-state
           restore): the same bytes again. Otherwise read it now, the file held
           before released first. A file that changed on disk since the index
           and no longer reads is refused out loud and the level is stock. */
        if (!g_mod.blob || std::strcmp(g_mod.shown, e->path) != 0)
            load_file(g_mod, e->path, e->mod, 0);
        if (g_mod.blob && std::strcmp(g_mod.shown, e->path) == 0) {
            if (g_mod.level != level_id) {
                char msg[1280];
                std::snprintf(msg, sizeof msg,
                              "%s (mod %s) was for level %d at boot and now "
                              "says level %d", g_mod.shown, e->mod, level_id,
                              g_mod.level);
                refuse(msg);
            } else {
                apply_sections(g_mod, ovl, level_id);
            }
        }
    } else if (g_env_state == 1) {
        char msg[1280];
        std::snprintf(msg, sizeof msg,
                      "%s is for level %d and this boot mounted level %d",
                      g_env.shown, g_env.level, level_id);
        refuse(msg);
    }
    dump_level_tables(level_id, ovl);
}
