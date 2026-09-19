//cpp
/* SHADOW translation unit -- ov002 / the level-object loader.
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build. The seventeen one-function sources under src/ remain the enrolled
 * owners of 0x020fe190..0x020fea4c. See
 * notes/translation-unit-reconstruction-plan.md (the design),
 * notes/tu-reconstruction-pilot-report.md (pilot #1, a key-function TU) and
 * notes/tu-reconstruction-pilot-2-report.md (this one).
 *
 * WHY THIS CANDIDATE: it is the deliberate opposite of pilot #1. There is no
 * class here at all -- fifteen of the seventeen functions are `_Z`-mangled FREE
 * functions -- so no destructor variant group, no vtable and no RTTI can be
 * emitted, which is the whole point of this round.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order. Re-probed against this exact
 * compiler and flags for this pilot rather than inherited from pilot #1
 * (four functions in, four sections out, strictly reversed).
 *
 * THAT REVERSAL IS ALSO EVIDENCE HERE, and it points the other way from what a
 * "reverse the ROM" instruction sounds like. Compiled section order equals the
 * ROM's address order and equals the reverse of source order, so the ORIGINAL
 * file's reading order was the reverse of the ROM's addresses -- which is this
 * file's own top-to-bottom order. Read that way it is exactly define-before-use:
 * the fifteen category loaders first, then LoadObjects which dispatches to them
 * through the table at ov002:0x0210cbb8, then Stage::LoadClsnAndObjects, which is
 * LoadObjects' only caller anywhere in the ROM. And after LoadSimpleObjects the
 * remaining fourteen come out in ascending handler-table index --
 * 0,1,2,3,4,6,7,8,9,10,11,12,13,14 -- with only index 5 hoisted to the front.
 * Nothing about that ordering was used to choose the boundary; it is what the
 * mechanical reversal produced, which is why it corroborates it.
 *
 * THIS TU ALSO OWNS, and this round neither declares nor verifies any of it:
 *   data_ov002_0210cbb8  .data  the 15-entry handler table (0x0210cbb8..0x0210cbf4)
 * and probably the small tables the loaders index (0x0210cb5c, 0x0210cb70,
 * 0x0210cb88, 0x0210cbf4) and the counter data_ov002_0211118c in .bss. Those are
 * a data-phase question; see the report.
 */
#include "decl_common.h"

#include "LVL_Overlay.h"
#include "Stage.h"
#include "dBgW.h"

/* ------------------------------------------------------------------------- *
 * Reconciled declarations.
 *
 * These are the seventeen files' `extern` blocks merged. Unlike pilot #1, where
 * every repeated declaration agreed, FOUR of these disagreed across files and
 * had to be decided; each decision is marked CONFLICT and argued at its site.
 * ------------------------------------------------------------------------- */
extern "C" {

/* CONFLICT 1 -- data_0209caa0's element type.
 * Stage::LoadClsnAndObjects declared `extern int data_0209caa0[]` and reads
 * `data_0209caa0[2] & 0x80`; LoadEntranceObjects declared `extern u8
 * data_0209caa0[]` and reads `data_0209caa0[0x41]`. Both cannot be the array's
 * element type. The byte view wins because it is the finer one -- index 0x41 is
 * not expressible as an int index -- and Stage's word read becomes an explicit
 * `((int *)data_0209caa0)[2]`, which is what the wider declaration was doing
 * implicitly. Measured byte-free on both functions. */
extern u8 data_0209caa0[];

/* CONFLICT 2 -- data_0209f5c0's type.
 * Stage said `fBase_c *`, LoadEntranceObjects said `void *` and cast it back
 * to `fBase_c *` at the one place it used it. fBase_c * is the real type
 * (it is the parent handed to dBase_c::Spawn), so the cast disappears. */
extern fBase_c *data_0209f5c0;

extern u8   data_0209f21c;
extern u8   data_0209f220;
extern u8   data_0209f250;
extern u8   data_0209f2d8;
extern void *data_0209f318;
extern int  data_0209f338[];
extern void *data_0209f394[];
extern u8   data_0209fc5c[];
extern int  data_02092134;
extern u8   data_02092128[];

extern s8   data_ov002_0210cb5c[];
extern u8   data_ov002_0210cb70[];
extern u16  data_ov002_0210cb88[];
extern u16  data_ov002_0210cbf4[];

/* CONFLICT 3 -- dActor_c::Spawn's spelling.
 * Five of the seventeen call it. Four spelled the mangled symbol by hand as an
 * `extern "C"` free function, and LoadStandardObjects records why: calling the
 * real `dActor_c::Spawn` -- whose header gives the last two arguments as s8/s16 --
 * inserts truncates that make that function four bytes too long, because
 * `areaID` arrives as a full-word int. LoadEntranceObjects used the real method.
 * One TU can only have one, and the hand-spelled form is the one the bytes
 * require, so LoadEntranceObjects moves to it. The four hand-spelled copies also
 * disagreed among themselves -- `const Vector3 *` versus `const Vector3 &` for
 * the position, `Vector3s` versus `Vector3_16` for the rotation -- so all five
 * call sites now go through this one declaration. Measured byte-free on all
 * five, including LoadEntranceObjects' move off the real method. */
void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
        u32 actorID, u32 spawnParam, const Vector3 *pos, const Vector3_16 *rot,
        s32 areaID, s32 deathTableID);

void _Z23LoadMinimapChangeObjecti5Fix12IiEh(int areaID, Fix12i y, s8 h);

void func_0202af80(void *entries, u32 count);
void func_0202b044(void *entries, u32 count);
void func_0202b060(void *entries, u32 count);
void func_0202b090(void *entries, u32 count);
void func_0202b0c4(void *entries, u32 count);
void func_0202b0e0(LVL_Overlay::StandardEntry *e, int count);
void StartEntranceFaderWipe(void);

/* Stage::LoadClsnAndObjects' own callees. LoadFile is not in decl_common.h. */
struct KCL_File;
struct CLPS_Block;
KCL_File *LoadFile(int handle);
void _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File(KCL_File *f);
void _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(dBgW_Kc *thiz,
                                                       KCL_File *f,
                                                       CLPS_Block *clps);
int  _ZNK7dBgW_Kc16GetOctreeOriginYEv(dBgW_Kc *thiz);
int  _ZNK7dBgW_Kc13GetUnkOctreeYEv(dBgW_Kc *thiz);

/* The handler table LoadObjects dispatches through. Believed to belong to this
 * TU (its fifteen entries are exactly the fifteen loaders below and nothing
 * else references them), but this round does not define it. */
extern void (*data_ov002_0210cbb8[])(void *, int, unsigned int);

}   /* extern "C" */

/* Per-category record types that include/LVL_Overlay.h does not carry yet.
 * The header's own banner says record types get added as their loader is
 * migrated; adding one is an include/ edit with its own blast radius over the
 * fifteen enrolled siblings, which this read-only pilot is not allowed to make.
 * So it stays here, and stays a TU-local declaration rather than a shadow of a
 * type that already exists elsewhere. */
struct DoorEntry {                  /* 0xc bytes. LoadDoorObjects. */
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    s16 angle;                      /* 0x06 */
    u16 field8;                     /* 0x08 */
    u16 fielda;                     /* 0x0a */
};

/* The runtime layout of an LVL_Overlay. include/LVL_Overlay.h declares only the
 * class's NESTED table and record types -- it has no data members at all -- so
 * the outer object's own shape is still a shadow. Only
 * Stage::LoadClsnAndObjects reads it. */
struct LVL_Overlay_Layout {
    CLPS_Block           *clps;         /* 0x00 */
    LVL_Overlay::ObjTable *objTable;    /* 0x04 */
    char                  pad_008[2];   /* 0x08 */
    u16                   kclFileId;    /* 0x0a */
    char                  pad_00c[4];   /* 0x0c */
    struct SubTbl {
        LVL_Overlay::ObjTable *table;
        char pad_004[8];
    }                    *subTables;    /* 0x10 */
    u8                    subCount;     /* 0x14 */
};

/* ========================================================================= *
 * ROM ordinal 16 -- LoadSimpleObjects, 0x020fe960, size 0xec
 * ========================================================================= */
/* Spawn simple actors, or hand a minimap-change sentinel to
 * LoadMinimapChangeObject. Each SimpleEntry is 8 bytes: a packed `raw` word
 * (low 9 bits = actor-table index, high bits = param) and an s16 position.
 * There is no rotation; spawn passes NULL for it. raw == 0x1ff is the
 * minimap-change special case. The third parameter is part of the shared loader
 * signature and unused here.
 *
 * LoadMinimapChangeObject stays hand-spelled: it takes a by-value Fix12<int>,
 * which is a runbook section 7 dead end for a real declaration at the call site. */
void LoadSimpleObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::SimpleEntry* e = (LVL_Overlay::SimpleEntry*)tbl.entries;
    int i;
    for (i = 0; i < (int)tbl.count; i++, e++) {
        Vector3 v;
        int raw;
        int actorParam;
        int masked;
        int zz = e->z << 12;
        int yy = e->y << 12;
        int xx = e->x << 12;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        raw = e->raw;
        masked = (u16)(raw & 0x1ff);
        actorParam = raw >> 9;
        if (masked == 0x1ff) {
            _Z23LoadMinimapChangeObjecti5Fix12IiEh(areaID, yy, (s8)(actorParam & 0xf));
        } else {
            s16 old = data_ov002_0211118c;
            data_ov002_0211118c = (s16)(old + 1);
            _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                data_ov002_0210cbf4[masked], actorParam, &v, 0, areaID, old);
        }
    }
}

/* ========================================================================= *
 * ROM ordinal 15 -- LoadStandardObjects, 0x020fe8ac, size 0xb4
 * ========================================================================= */
/* One actor per standard object entry. Each StandardEntry is 0x10 bytes: raw id
 * index into data_ov002_0210cbf4, s16 position (shifted left by 12 into Fix12),
 * Vector3s rotation, u16 param. data_ov002_0211118c is a running death-table /
 * spawn-order counter incremented per spawn. The third parameter is part of the
 * shared loader signature and unused here. */
void LoadStandardObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::StandardEntry* e = (LVL_Overlay::StandardEntry*)tbl.entries;
    int i;
    for (i = 0; i < (int)tbl.count; i++, e++) {
        Vector3 v;
        u32 a = data_ov002_0210cbf4[e->raw];
        int zz = e->z << 12;
        int yy = e->y << 12;
        int xx = e->x << 12;
        v.x = xx;
        v.y = yy;
        v.z = zz;
        s16 old = data_ov002_0211118c;
        data_ov002_0211118c = (s16)(old + 1);
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            a, e->param, &v, (const Vector3_16*)&e->rot, areaID, old);
    }
}

/* ========================================================================= *
 * ROM ordinal 14 -- LoadEntranceObjects, 0x020fe6c8, size 0x1e4
 * ========================================================================= */
/* Spawn entrance actors and the entrance-controller dBase_c. Walks
 * StandardEntry records like LoadStandardObjects, but the third argument is an
 * entry index OFFSET into the table rather than a free parameter: the walk
 * starts at `entries + p3` and runs for data_0209f21c iterations, not tbl.count. */
void LoadEntranceObjects(LVL_Overlay::ObjSubTable& tbl, int p2, u32 p3)
{
    u32 sl;
    LVL_Overlay::StandardEntry* e = (LVL_Overlay::StandardEntry*)tbl.entries;
    func_0202b0e0(e, tbl.count);
    e += p3;
    sl = 0;

    int entranceId = 0xf;
    u8 i = 0;

    while (i < data_0209f21c) {
        {
            Vector3 pos;
            int vz = e->z << 12;
            int vy = e->y << 12;
            int vx = e->x << 12;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;

            u16 param = e->param;
            sl = (param >> 7) & 0xf;

            int f2 = data_0209caa0[0x41];
            int f1 = data_02092128[i];
            int cond = (data_0209f2d8 == 1);
            if (cond != 0) {
                f2 = 3;
                f1 = 3;
            }
            u32 flags = f2 | (f1 << 3) | (i << 6) | (sl << 8);

            void* a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                data_ov002_0210cbf4[e->raw], flags, &pos,
                (const Vector3_16*)&e->rot, (s8)(param & 7), -1);

            if (data_0209fc5c[i] == 0)
                data_0209f394[i] = 0;
            else
                data_0209f394[i] = a;

            if (i == data_0209f250)
                entranceId = (e->param >> 3) & 0xf;

            e++;
            i++;
        }
    }

    data_0209f318 = dBase_c::Spawn(0x14c, data_0209f5c0, entranceId, 0);

    int t = data_0209f2d8;
    t = t == 2;
    if (t != 0)
        return;

    if (sl >= 0x13)
        sl = 0;
    if (data_ov002_0210cb5c[sl] < 0)
        return;

    StartEntranceFaderWipe();
}

/* ========================================================================= *
 * ROM ordinal 13 -- LoadPathNodeObjects, 0x020fe6b8, size 0x10
 * ========================================================================= */
/* CONFLICT 4 -- func_0203accc's arity, and this is the one consolidation could
 * not paper over.
 *
 * This file declared `void func_0203accc(void *entries, int areaID, u32 param)`
 * and called it with three arguments; Stage::LoadClsnAndObjects (bottom of this
 * file) includes decl_common.h, which declares `void func_0203accc(int)`, and
 * calls it with one. Two `extern "C"` declarations of one symbol with different
 * arity cannot coexist, and no cast reconciles them.
 *
 * The ROM decides it. Stage's call site is `mov r0, #0` then `bl` -- r1 and r2
 * are never set -- so a three-argument declaration would cost two extra
 * instructions there. This function is a tail call, `ldr r0, [r0, #4]` then
 * `bx ip`, which passes r1/r2 through by accident of the ABI rather than by
 * intent: the same four instructions come out whether the callee is declared to
 * take them or not. So the one-argument declaration is the one that can be
 * true of both call sites, and the three-argument spelling was an inference
 * from the tail-call shape that the other caller contradicts. Measured
 * byte-free here. */
void LoadPathNodeObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0203accc((int)tbl.entries);
}

/* ========================================================================= *
 * ROM ordinal 12 -- LoadPathObjects, 0x020fe6a4, size 0x14
 * ========================================================================= */
/* Hand the category's entry array and count straight to func_0203aca0.
 * decl_common.h declares that one `(int, int)`; this file used to declare it
 * `(void *, u32)`. Same arity, so a cast settles it -- unlike func_0203accc
 * above. */
void LoadPathObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0203aca0((int)tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 11 -- LoadViewObjects, 0x020fe690, size 0x14
 * ========================================================================= */
void LoadViewObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b0c4(tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 10 -- LoadTeleportSourceObjects, 0x020fe5f4, size 0x9c
 * ========================================================================= */
/* One teleport-source actor (id 0x15b) per entry. Simpler than its exit-loader
 * sibling in two ways visible in the record: the parameter is a plain u16 rather
 * than a big-endian byte quartet, and there is no rotation at all -- the spawn
 * call passes NULL for it. The record is 8 bytes against the exit record's 0xe.
 *
 * The loop is bottom-tested with a guard rather than top-tested, and that shape
 * is load-bearing: it is what the ROM emits, and a plain `for` reorders the
 * compare. */
void LoadTeleportSourceObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::TeleportSourceEntry* e =
        (LVL_Overlay::TeleportSourceEntry*)tbl.entries;

    s32 i = 0;
    if ((int)tbl.count <= 0)
        return;

    do
    {
        Vector3 pos;
        s32 vz = e->z << 12;
        s32 vy = e->y << 12;
        s32 vx = e->x << 12;
        pos.x = vx;
        pos.y = vy;
        pos.z = vz;

        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x15b, e->param, &pos, (const Vector3_16*)0, 0, -1);

        i++;
        e++;
    } while (i < (int)tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 9 -- LoadTeleportDestObjects, 0x020fe5e0, size 0x14
 * ========================================================================= */
void LoadTeleportDestObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b090(tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 8 -- LoadFogObjects, 0x020fe5cc, size 0x14
 * ========================================================================= */
void LoadFogObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b060(tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 7 -- LoadDoorObjects, 0x020fe4f0, size 0xdc
 * ========================================================================= */
/* One door actor per entry. The actor id and the high half of its parameter come
 * out of two small ov002 tables indexed by the low five bits of the record's
 * last halfword; the rotation is unpacked from two nibbles of field8 plus the
 * stored angle.
 *
 * This is the file that carried the most local shadow: its own `Vector3`,
 * `Vector3_16`, `DoorEntry` and -- uniquely among the seventeen -- its own
 * `class LVL_Overlay` with a THIRD spelling of ObjSubTable (`u8 b0; u8 count;
 * DoorEntry *entries;`). That spelling is layout-compatible with the header's,
 * so only the names change here. */
void LoadDoorObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    DoorEntry* e = (DoorEntry*)tbl.entries;
    for (int i = 0; i < tbl.count; i++) {
        Vector3 pos;
        int z = e->z << 12;
        int y = e->y << 12;
        int x = e->x << 12;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        Vector3_16 rot;
        s16 rx = (e->field8 >> 8) & 0xf;
        s16 ry = e->angle;
        s16 rz = (e->field8 >> 12) & 0xf;
        rot.x = rx;
        rot.y = ry;
        rot.z = rz;
        int idx = e->fielda & 0x1f;
        u32 id = data_ov002_0210cb88[idx];
        u32 p = e->field8 | (data_ov002_0210cb70[idx] << 16);
        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(id, p, &pos, &rot, -1, -1);
        e++;
    }
}

/* ========================================================================= *
 * ROM ordinal 6 -- LoadExitObjects, 0x020fe420, size 0xd0
 * ========================================================================= */
/* One exit actor (id 0x15d) per entry. Rotations are stored the same way as
 * positions and NEGATED -- both of them -- so a stored angle is the opposite of
 * the one the actor gets.
 *
 * THE PARAMETER WORD IS STORED BIG-ENDIAN: bytes 0x0a..0x0d assemble as
 * `(b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]`, which on a little-endian
 * machine is the reverse of a plain load. The ROM really does shift 0x0a by 24,
 * which is why the header keeps the field a four-byte array. */
void LoadExitObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    LVL_Overlay::ExitEntry* e = (LVL_Overlay::ExitEntry*)tbl.entries;

    for (int i = 0; i < (int)tbl.count; i++)
    {
        Vector3 pos;
        s32 pz = e->z << 12;
        s32 py = e->y << 12;
        s32 px = e->x << 12;
        pos.x = px;
        pos.y = py;
        pos.z = pz;

        Vector3s rot;
        rot.x = -e->rotX;
        rot.y = -e->rotY;

        _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            0x15d,
            ((u32)e->param[0] << 24) | ((u32)e->param[1] << 16)
                | ((u32)e->param[2] << 8) | (u32)e->param[3],
            &pos, (const Vector3_16*)&rot, -1, -1);

        e++;
    }
}

/* ========================================================================= *
 * ROM ordinal 5 -- LoadMinimapTileObjects, 0x020fe40c, size 0x14
 * ========================================================================= */
void LoadMinimapTileObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b044(tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 4 -- LoadMinimapScaleObjects, 0x020fe3f8, size 0x14
 * ========================================================================= */
void LoadMinimapScaleObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202af80(tbl.entries, tbl.count);
}

/* ========================================================================= *
 * ROM ordinal 3 -- LoadUnusedType13Objects, 0x020fe3e4, size 0x14
 * ========================================================================= */
/* Stash the entry array's ADDRESS and do nothing with it. Not a mistake in the
 * recovery: the ROM really does store the pointer rather than walk it. Object
 * type 13 is unused in the shipped game, so the loader was reduced to a stub
 * that keeps the table around, which also explains why it reads no count. */
void LoadUnusedType13Objects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_0209f338[0] = (int)tbl.entries;
}

/* ========================================================================= *
 * ROM ordinal 2 -- LoadStarCameraObjects, 0x020fe3cc, size 0x18
 * ========================================================================= */
/* Take the first word of the star-camera table and publish it. Reads no count,
 * unlike most of this family: it wants one value, not a list. */
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_02092134 = *(int*)tbl.entries;
}

/* ========================================================================= *
 * ROM ordinal 1 -- LoadObjects, 0x020fe33c, size 0x90
 * ========================================================================= */
/* Walk the outer object table and dispatch each 8-byte entry to the category
 * loader table at data_ov002_0210cbb8 -- whose fifteen entries are exactly the
 * fifteen functions above.
 *
 * Count is a u16 at offset 0 (unlike ObjSubTable's u8 at offset 1). Each entry
 * is 8 bytes; byte 0 packs a filter in the top three bits and a handler index in
 * the low five. The entry is skipped unless the filter is 0 or equal to the
 * global byte at data_0209f220. */
void LoadObjects(LVL_Overlay::ObjTable& t, int areaID, u32 param)
{
    u8* e = (u8*)t.entries;
    int i;
    for (i = 0; i < (int)t.count; i++) {
        u8 b = *e;
        int type = (b >> 5) & 7;
        if (type == 0 || type == data_0209f220) {
            void (*h)(void*, int, unsigned int) = data_ov002_0210cbb8[b & 0x1f];
            if (h != 0)
                h(e, areaID, param);
        }
        e += 8;
    }
}

/* ========================================================================= *
 * ROM ordinal 0 -- Stage::LoadClsnAndObjects, 0x020fe190, size 0x1ac
 * ========================================================================= */
/* The public entry point, and LoadObjects' only caller anywhere in the ROM:
 * load the level's collision mesh, then run the object tables, then decide
 * whether the intro cutscene plays.
 *
 * In the one-function tree this called LoadObjects through the hand-spelled
 * `_Z11LoadObjectsRN11LVL_Overlay8ObjTableEij`, because that was the only way to
 * reach a symbol defined in a different file. Here LoadObjects is a real
 * function in the same TU, so it is called by name -- and the hand-spelled
 * extern had to go, since an `extern "C"` declaration of that literal name and
 * the C++ definition that mangles to it are the same linker symbol. */
void Stage::LoadClsnAndObjects(LVL_Overlay &ovlRef, u32 p, dBgW_Kc &mcRef)
{
    LVL_Overlay_Layout *ovl = (LVL_Overlay_Layout *)&ovlRef;
    dBgW_Kc *mc = (dBgW_Kc *)&mcRef;
    KCL_File* f;
    LVL_Overlay_Layout::SubTbl* e;
    signed char i;
    int kuppa;
    int intro;
    int t1;
    int t2;
    int isTwo;

    func_0203accc(0);
    func_0203aca0(0, 0);

    if (ovl->kclFileId != 0) {
        f = LoadFile(ovl->kclFileId);
        _ZN7dBgW_Kc17UpdateFileOffsetsER8KCL_File(f);
        _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block(mc, f, ovl->clps);
        func_0202a850(_ZNK7dBgW_Kc16GetOctreeOriginYEv(mc), _ZNK7dBgW_Kc13GetUnkOctreeYEv(mc));
        ((dBgW *)(mc))->Enable((dActor_c *)(0));
    }

    data_ov002_0211118c = 0;
    LoadObjects(*ovl->objTable, -1, p);

    e = ovl->subTables;
    for (i = 0; i < ovl->subCount; i++) {
        if (e->table != 0)
            LoadObjects(*e->table, i, p);
        e++;
    }

    kuppa = ContinueKuppaScriptIfNecessary();

    intro = 0;
    t2 = 0;
    t1 = (data_0209f2d8 == 0);
    if (t1 != 0) {
        if ((((int *)data_0209caa0)[2] & 0x80) == 0)
            t2 = 1;
    }
    if (t2 != 0) {
        if (kuppa == 0)
            intro = 1;
    }

    isTwo = (data_0209f2d8 == 2);
    if (isTwo == 0) {
        dBase_c::Spawn(0x14f, data_0209f5c0, 0, 0);
        if (intro == 0)
            dBase_c::Spawn(0x14e, data_0209f5c0, 0, 0);
    }

    if (intro != 0)
        StartIntroCutscene();
}
