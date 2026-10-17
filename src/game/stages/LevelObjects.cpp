//cpp
/* leftovers:
 * - Source order stays highest address first. Reordering moves the run.
 * - data_ov002_0210cbb8 and the small loader tables stay data. This TU
 *   claims .text only.
 * - DoorEntry and LVL_Overlay_Layout stay in this file. LVL_Overlay.h
 *   still has no data members, and adding a record type there reaches
 *   the other loaders.
 * - dActor_c::Spawn stays the hand-spelled form. The header's s8/s16
 *   tail truncates areaID and grows LoadStandardObjects by 4 bytes.
 * - LoadMinimapChangeObject stays mangled. It takes Fix12<int> by value.
 * - LoadTeleportSourceObjects stays a bottom-tested loop. A for
 *   reorders the compare.
 * - LoadExitObjects keeps the big-endian parameter assembly. A plain
 *   load would reverse the bytes.
 *
 * Fifteen category loaders, then LoadObjects, then
 * dScStage_c::LoadClsnAndObjects. ov002 .text 0x020fe190..0x020fea4c.
 */
#include "decl_common.h"

#include "LVL_Overlay.h"
#include "dScStage_c.h"
#include "dBgW.h"
#include "PathPtr.h"
#include "Vector3s.h"

/* The old per-loader files disagreed on four externs. One TU can keep
 * one spelling of each. */
extern "C" {

/* data_0209caa0 is bytes. LoadEntranceObjects indexes [0x41].
 * LoadClsnAndObjects reads word 2 through an int*. */
extern u8 data_0209caa0[];

/* data_0209f5c0 is the parent passed to dBase_c::Spawn. */
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

/* The header's s8/s16 tail truncates areaID and adds 4 bytes to
 * LoadStandardObjects. All five call sites use this spelling.
 * local extern: s32 areaID/deathTableID, see above. */
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
void StartEntranceFaderWipe(int index);

/* dScStage_c::LoadClsnAndObjects' own callees. LoadFile is not in decl_common.h. */
struct KCL_File;
struct CLPS_Block;
KCL_File *LoadFile(int handle);

/* LoadObjects dispatches through this table. Its entries are the
 * category loaders. This text-only TU does not define it. */
extern void (*data_ov002_0210cbb8[])(void *, int, unsigned int);

}   /* extern "C" */

/* Door records. LVL_Overlay.h does not carry this type, and adding it
 * there would recompile every loader that includes the header. */
struct DoorEntry {                  /* 0xc bytes. LoadDoorObjects. */
    s16 x;                          /* 0x00 */
    s16 y;                          /* 0x02 */
    s16 z;                          /* 0x04 */
    s16 angle;                      /* 0x06 */
    u16 field8;                     /* 0x08 */
    u16 fielda;                     /* 0x0a */
};

/* The overlay object as LoadClsnAndObjects reads it. LVL_Overlay.h
 * declares the nested tables and no data members. */
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

/* The loaders test the game mode as a 0/1 value. */
static inline int IsGameMode(int mode) {
    return data_0209f2d8 == mode;
}

/* Game mode 0 with bit 0x80 of data_0209caa0 word 2 clear. */
static inline int IntroAllowed() {
    return IsGameMode(0) && (((int *)data_0209caa0)[2] & 0x80) == 0;
}

/* Spawn simple actors, or hand a minimap-change sentinel to
 * LoadMinimapChangeObject. Each SimpleEntry is 8 bytes: a packed `raw` word
 * (low 9 bits = actor-table index, high bits = param) and an s16 position.
 * There is no rotation; spawn passes NULL for it. raw == 0x1ff is the
 * minimap-change special case. The third parameter is part of the shared loader
 * signature and unused here.
 *
 * LoadMinimapChangeObject stays hand-spelled: it takes a by-value Fix12<int>,
 * which is a runbook section 7 dead end for a real declaration at the call site. */
// @symbol _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEij
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

/* One actor per standard object entry. Each StandardEntry is 0x10 bytes: raw id
 * index into data_ov002_0210cbf4, s16 position (shifted left by 12 into Fix12),
 * Vector3s rotation, u16 param. data_ov002_0211118c is a running death-table /
 * spawn-order counter incremented per spawn. The third parameter is part of the
 * shared loader signature and unused here. */
// @symbol _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEij
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

/* Spawn entrance actors and the entrance-controller dBase_c. Walks
 * StandardEntry records like LoadStandardObjects, but the third argument is an
 * entry index OFFSET into the table rather than a free parameter: the walk
 * starts at `entries + p3` and runs for data_0209f21c iterations, not tbl.count. */
// @symbol _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEij
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
            if (IsGameMode(1)) {
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

    if (IsGameMode(2))
        return;

    if (sl >= 0x13)
        sl = 0;
    int wipe = data_ov002_0210cb5c[sl];
    if (wipe < 0)
        return;

    StartEntranceFaderWipe(wipe);
}

// @symbol _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadPathNodeObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    PathPtr::SetNodeTable(tbl.entries);
}

// @symbol _Z15LoadPathObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadPathObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    PathPtr::SetPathTable((PathDef*)tbl.entries, tbl.count);
}

// @symbol _Z15LoadViewObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadViewObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b0c4(tbl.entries, tbl.count);
}

/* One teleport-source actor (id 0x15b) per entry. Simpler than its exit-loader
 * sibling in two ways visible in the record: the parameter is a plain u16 rather
 * than a big-endian byte quartet, and there is no rotation at all -- the spawn
 * call passes NULL for it. The record is 8 bytes against the exit record's 0xe.
 *
 * The loop is bottom-tested with a guard rather than top-tested, and that shape
 * is load-bearing: it is what the ROM emits, and a plain `for` reorders the
 * compare. */
// @symbol _Z25LoadTeleportSourceObjectsRN11LVL_Overlay11ObjSubTableEij
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

// @symbol _Z23LoadTeleportDestObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadTeleportDestObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b090(tbl.entries, tbl.count);
}

// @symbol _Z14LoadFogObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadFogObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b060(tbl.entries, tbl.count);
}

/* One door actor per entry. The actor id and the high half of its parameter come
 * out of two small ov002 tables indexed by the low five bits of the record's
 * last halfword; the rotation is unpacked from two nibbles of field8 plus the
 * stored angle. */
// @symbol _Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEij
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

/* One exit actor (id 0x15d) per entry. Rotations are stored the same way as
 * positions and NEGATED -- both of them -- so a stored angle is the opposite of
 * the one the actor gets.
 *
 * THE PARAMETER WORD IS STORED BIG-ENDIAN: bytes 0x0a..0x0d assemble as
 * `(b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3]`, which on a little-endian
 * machine is the reverse of a plain load. The ROM really does shift 0x0a by 24,
 * which is why the header keeps the field a four-byte array. */
// @symbol _Z15LoadExitObjectsRN11LVL_Overlay11ObjSubTableEij
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

// @symbol _Z22LoadMinimapTileObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadMinimapTileObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202b044(tbl.entries, tbl.count);
}

// @symbol _Z23LoadMinimapScaleObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadMinimapScaleObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    func_0202af80(tbl.entries, tbl.count);
}

/* Stash the entry array's ADDRESS and do nothing with it. Not a mistake in the
 * recovery: the ROM really does store the pointer rather than walk it. Object
 * type 13 is unused in the shipped game, so the loader was reduced to a stub
 * that keeps the table around, which also explains why it reads no count. */
// @symbol _Z23LoadUnusedType13ObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadUnusedType13Objects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_0209f338[0] = (int)tbl.entries;
}

/* Take the first word of the star-camera table and publish it. Reads no count,
 * unlike most of this family: it wants one value, not a list. */
// @symbol _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEij
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable& tbl, int areaID, u32 param)
{
    data_02092134 = *(int*)tbl.entries;
}

/* Walk the outer object table and dispatch each 8-byte entry to the category
 * loader table at data_ov002_0210cbb8 -- whose fifteen entries are exactly the
 * fifteen functions above.
 *
 * Count is a u16 at offset 0 (unlike ObjSubTable's u8 at offset 1). Each entry
 * is 8 bytes; byte 0 packs a filter in the top three bits and a handler index in
 * the low five. The entry is skipped unless the filter is 0 or equal to the
 * global byte at data_0209f220. */
// @symbol _Z11LoadObjectsRN11LVL_Overlay8ObjTableEij
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

/* Loads the collision mesh, runs the object tables, then decides
 * whether the intro cutscene plays. LoadObjects is in this TU, so the
 * call is by name. */
// @symbol _ZN10dScStage_c18LoadClsnAndObjectsER11LVL_OverlayjR7dBgW_Kc
void dScStage_c::LoadClsnAndObjects(LVL_Overlay &ovlRef, u32 p, dBgW_Kc &mcRef)
{
    LVL_Overlay_Layout *ovl = (LVL_Overlay_Layout *)&ovlRef;
    dBgW_Kc *mc = (dBgW_Kc *)&mcRef;
    KCL_File* f;
    LVL_Overlay_Layout::SubTbl* e;
    signed char i;
    int kuppa;
    int intro;

    PathPtr::SetNodeTable(0);
    PathPtr::SetPathTable(0, 0);

    if (ovl->kclFileId != 0) {
        f = LoadFile(ovl->kclFileId);
        dBgW_Kc::UpdateFileOffsets(*f);
        ((dBgW_Kc *)mc)->SetFile(f, *ovl->clps);
        func_0202a850(((dBgW_Kc *)mc)->GetOctreeOriginY(), ((dBgW_Kc *)mc)->GetUnkOctreeY());
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

    intro = IntroAllowed() && kuppa == 0;

    if (!IsGameMode(2)) {
        dBase_c::Spawn(0x14f, data_0209f5c0, 0, 0);
        if (intro == 0)
            dBase_c::Spawn(0x14e, data_0209f5c0, 0, 0);
    }

    if (intro != 0)
        StartIntroCutscene();
}
