//cpp
/* leftovers:
 * - Source order stays highest address first. Reordering moves the run.
 * - data_ov002_0210cbb8 and the small loader tables stay data. This TU
 *   claims .text only.
 * - DoorEntry and LVL_Overlay_Layout stay in this file. LVL_Overlay.h
 *   still has no data members, and adding a record type there reaches
 *   the other loaders.
 * - areaID is s8 down the whole chain: LoadClsnAndObjects counts areas in
 *   a signed char, LoadObjects and the fifteen category loaders take it as
 *   s8, and dActor_c::Spawn's s8 area tail receives it unconverted.
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
#include "dActor_c.h"
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

extern u8 data_0209f21c;
extern u8 data_0209f220;
extern u8 data_0209f250;
extern u8 data_0209f2d8;
extern void *data_0209f318;
extern int data_0209f338[];
extern void *data_0209f394[];
extern u8 data_0209fc5c[];
extern int data_02092134;
extern u8 data_02092128[];

extern s8 data_ov002_0210cb5c[];
extern u8 data_ov002_0210cb70[];
extern u16 data_ov002_0210cb88[];
extern u16 data_ov002_0210cbf4[];

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
extern void (*data_ov002_0210cbb8[])(void *, s8, unsigned int);

} /* extern "C" */

/* Door records. LVL_Overlay.h does not carry this type, and adding it
 * there would recompile every loader that includes the header. */
struct DoorEntry { /* 0xc bytes. LoadDoorObjects. */
    s16 x;         /* 0x00 */
    s16 y;         /* 0x02 */
    s16 z;         /* 0x04 */
    s16 angle;     /* 0x06 */
    u16 param;     /* 0x08 - packed spawn parameter and rotation nibbles */
    u16 type;      /* 0x0a - low five bits select the door tables */
};

/* The overlay object as LoadClsnAndObjects reads it. LVL_Overlay.h
 * declares the nested tables and no data members. */
struct LVL_Overlay_Layout {
    CLPS_Block *clps;                /* 0x00 */
    LVL_Overlay::ObjTable *objTable; /* 0x04 */
    char pad_008[2];                 /* 0x08 */
    u16 kclFileId;                   /* 0x0a */
    char pad_00c[4];                 /* 0x0c */
    struct SubTbl {
        LVL_Overlay::ObjTable *table;
        char pad_004[8];
    } *subTables; /* 0x10 */
    u8 subCount;  /* 0x14 */
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
 * signature and unused here. */
// @symbol _Z17LoadSimpleObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadSimpleObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::SimpleEntry *entry = (LVL_Overlay::SimpleEntry *)table.entries;
    int objectIndex;
    for (objectIndex = 0; objectIndex < (int)table.count; objectIndex++, entry++) {
        Vector3 position;
        int raw;
        int actorParam;
        int actorIndex;
        int z = entry->z << 12;
        int y = entry->y << 12;
        int x = entry->x << 12;
        position.x = x;
        position.y = y;
        position.z = z;
        raw = entry->raw;
        actorIndex = (u16)(raw & 0x1ff);
        actorParam = raw >> 9;
        if (actorIndex == 0x1ff) {
            LoadMinimapChangeObject(areaID, y, actorParam & 0xf);
        } else {
            s16 deathTableID = data_ov002_0211118c;
            data_ov002_0211118c = (s16)(deathTableID + 1);
            dActor_c::Spawn(data_ov002_0210cbf4[actorIndex], actorParam, position, 0, areaID,
                            deathTableID);
        }
    }
}

/* One actor per standard object entry. Each StandardEntry is 0x10 bytes: raw id
 * index into data_ov002_0210cbf4, s16 position (shifted left by 12 into Fix12),
 * Vector3s rotation, u16 param. data_ov002_0211118c is a running death-table /
 * spawn-order counter incremented per spawn. The third parameter is part of the
 * shared loader signature and unused here. */
// @symbol _Z19LoadStandardObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadStandardObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::StandardEntry *entry = (LVL_Overlay::StandardEntry *)table.entries;
    int objectIndex;
    for (objectIndex = 0; objectIndex < (int)table.count; objectIndex++, entry++) {
        Vector3 position;
        u32 actorID = data_ov002_0210cbf4[entry->raw];
        int z = entry->z << 12;
        int y = entry->y << 12;
        int x = entry->x << 12;
        position.x = x;
        position.y = y;
        position.z = z;
        s16 deathTableID = data_ov002_0211118c;
        data_ov002_0211118c = (s16)(deathTableID + 1);
        dActor_c::Spawn(actorID, entry->param, position, (const Vector3_16 *)&entry->rot, areaID,
                        deathTableID);
    }
}

/* Spawn entrance actors and the entrance-controller dBase_c. Walks
 * StandardEntry records like LoadStandardObjects, but the third argument is an
 * entry index OFFSET into the table rather than a free parameter: the walk
 * starts at `entries + entranceIndex` and runs for data_0209f21c iterations, not table.count. */
// @symbol _Z19LoadEntranceObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadEntranceObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 entranceIndex) {
    u32 entranceKind;
    LVL_Overlay::StandardEntry *entry = (LVL_Overlay::StandardEntry *)table.entries;
    func_0202b0e0(entry, table.count);
    entry += entranceIndex;
    entranceKind = 0;

    int entranceId = 0xf;
    u8 playerIndex = 0;

    while (playerIndex < data_0209f21c) {
        {
            Vector3 pos;
            int vz = entry->z << 12;
            int vy = entry->y << 12;
            int vx = entry->x << 12;
            pos.x = vx;
            pos.y = vy;
            pos.z = vz;

            u16 param = entry->param;
            entranceKind = (param >> 7) & 0xf;

            int savedCharacter = data_0209caa0[0x41];
            int playerCharacter = data_02092128[playerIndex];
            if (IsGameMode(1)) {
                savedCharacter = 3;
                playerCharacter = 3;
            }
            u32 spawnParam =
                savedCharacter | (playerCharacter << 3) | (playerIndex << 6) | (entranceKind << 8);

            void *player = dActor_c::Spawn(data_ov002_0210cbf4[entry->raw], spawnParam, pos,
                                           (const Vector3_16 *)&entry->rot, (s8)(param & 7), -1);

            if (data_0209fc5c[playerIndex] == 0)
                data_0209f394[playerIndex] = 0;
            else
                data_0209f394[playerIndex] = player;

            if (playerIndex == data_0209f250)
                entranceId = (entry->param >> 3) & 0xf;

            entry++;
            playerIndex++;
        }
    }

    data_0209f318 = dBase_c::Spawn(0x14c, data_0209f5c0, entranceId, 0);

    if (IsGameMode(2))
        return;

    if (entranceKind >= 0x13)
        entranceKind = 0;
    int wipeType = data_ov002_0210cb5c[entranceKind];
    if (wipeType < 0)
        return;

    StartEntranceFaderWipe(wipeType);
}

// @symbol _Z19LoadPathNodeObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadPathNodeObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    PathPtr::SetNodeTable(table.entries);
}

// @symbol _Z15LoadPathObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadPathObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    PathPtr::SetPathTable((PathDef *)table.entries, table.count);
}

// @symbol _Z15LoadViewObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadViewObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b0c4(table.entries, table.count);
}

/* One teleport-source actor (id 0x15b) per entry. Simpler than its exit-loader
 * sibling in two ways visible in the record: the parameter is a plain u16 rather
 * than a big-endian byte quartet, and there is no rotation at all -- the spawn
 * call passes NULL for it. The record is 8 bytes against the exit record's 0xe.
 *
 * The loop is bottom-tested with a guard rather than top-tested, and that shape
 * is load-bearing: it is what the ROM emits, and a plain `for` reorders the
 * compare. */
// @symbol _Z25LoadTeleportSourceObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadTeleportSourceObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::TeleportSourceEntry *entry = (LVL_Overlay::TeleportSourceEntry *)table.entries;

    s32 objectIndex = 0;
    if ((int)table.count <= 0)
        return;

    do {
        Vector3 pos;
        s32 vz = entry->z << 12;
        s32 vy = entry->y << 12;
        s32 vx = entry->x << 12;
        pos.x = vx;
        pos.y = vy;
        pos.z = vz;

        dActor_c::Spawn(0x15b, entry->param, pos, (const Vector3_16 *)0, 0, -1);

        objectIndex++;
        entry++;
    } while (objectIndex < (int)table.count);
}

// @symbol _Z23LoadTeleportDestObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadTeleportDestObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b090(table.entries, table.count);
}

// @symbol _Z14LoadFogObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadFogObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b060(table.entries, table.count);
}

/* One door actor per entry. The actor id and the high half of its parameter come
 * out of two small ov002 tables indexed by the low five bits of the record's
 * last halfword; the rotation is unpacked from two nibbles of param plus the
 * stored angle. */
// @symbol _Z15LoadDoorObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadDoorObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    DoorEntry *entry = (DoorEntry *)table.entries;
    for (int objectIndex = 0; objectIndex < table.count; objectIndex++) {
        Vector3 pos;
        int z = entry->z << 12;
        int y = entry->y << 12;
        int x = entry->x << 12;
        pos.x = x;
        pos.y = y;
        pos.z = z;
        Vector3_16 rot;
        s16 rx = (entry->param >> 8) & 0xf;
        s16 ry = entry->angle;
        s16 rz = (entry->param >> 12) & 0xf;
        rot.x = rx;
        rot.y = ry;
        rot.z = rz;
        int doorType = entry->type & 0x1f;
        u32 actorID = data_ov002_0210cb88[doorType];
        u32 spawnParam = entry->param | (data_ov002_0210cb70[doorType] << 16);
        dActor_c::Spawn(actorID, spawnParam, pos, &rot, -1, -1);
        entry++;
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
// @symbol _Z15LoadExitObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadExitObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::ExitEntry *entry = (LVL_Overlay::ExitEntry *)table.entries;

    for (int objectIndex = 0; objectIndex < (int)table.count; objectIndex++) {
        Vector3 pos;
        s32 pz = entry->z << 12;
        s32 py = entry->y << 12;
        s32 px = entry->x << 12;
        pos.x = px;
        pos.y = py;
        pos.z = pz;

        Vector3s rot;
        rot.x = -entry->rotX;
        rot.y = -entry->rotY;

        dActor_c::Spawn(0x15d,
                        ((u32)entry->param[0] << 24) | ((u32)entry->param[1] << 16) |
                            ((u32)entry->param[2] << 8) | (u32)entry->param[3],
                        pos, (const Vector3_16 *)&rot, -1, -1);

        entry++;
    }
}

// @symbol _Z22LoadMinimapTileObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadMinimapTileObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b044(table.entries, table.count);
}

// @symbol _Z23LoadMinimapScaleObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadMinimapScaleObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202af80(table.entries, table.count);
}

/* Stash the entry array's ADDRESS and do nothing with it. Not a mistake in the
 * recovery: the ROM really does store the pointer rather than walk it. Object
 * type 13 is unused in the shipped game, so the loader was reduced to a stub
 * that keeps the table around, which also explains why it reads no count. */
// @symbol _Z23LoadUnusedType13ObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadUnusedType13Objects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    data_0209f338[0] = (int)table.entries;
}

/* Take the first word of the star-camera table and publish it. Reads no count,
 * unlike most of this family: it wants one value, not a list. */
// @symbol _Z21LoadStarCameraObjectsRN11LVL_Overlay11ObjSubTableEaj
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    data_02092134 = *(int *)table.entries;
}

/* Walk the outer object table and dispatch each 8-byte entry to the category
 * loader table at data_ov002_0210cbb8 -- whose fifteen entries are exactly the
 * fifteen functions above.
 *
 * Count is a u16 at offset 0 (unlike ObjSubTable's u8 at offset 1). Each entry
 * is 8 bytes; byte 0 packs a star filter in the top three bits and a handler index in
 * the low five. The entry is skipped unless the filter is 0 or equal to the
 * selected star at data_0209f220. */
// @symbol _Z11LoadObjectsRN11LVL_Overlay8ObjTableEaj
void LoadObjects(LVL_Overlay::ObjTable &table, s8 areaID, u32 param) {
    u8 *entry = (u8 *)table.entries;
    int tableIndex;
    for (tableIndex = 0; tableIndex < (int)table.count; tableIndex++) {
        u8 categoryAndStar = *entry;
        int starFilter = (categoryAndStar >> 5) & 7;
        if (starFilter == 0 || starFilter == data_0209f220) {
            void (*loader)(void *, s8, unsigned int) = data_ov002_0210cbb8[categoryAndStar & 0x1f];
            if (loader != 0)
                loader(entry, areaID, param);
        }
        entry += 8;
    }
}

/* Loads the collision mesh, runs the object tables, then decides
 * whether the intro cutscene plays. LoadObjects is in this TU, so the
 * call is by name. */
// @symbol _ZN10dScStage_c18LoadClsnAndObjectsER11LVL_OverlayjR7dBgW_Kc
void dScStage_c::LoadClsnAndObjects(LVL_Overlay &overlayRef, u32 entranceIndex,
                                    dBgW_Kc &meshColliderRef) {
    LVL_Overlay_Layout *overlay = (LVL_Overlay_Layout *)&overlayRef;
    dBgW_Kc *meshCollider = (dBgW_Kc *)&meshColliderRef;
    KCL_File *collisionFile;
    LVL_Overlay_Layout::SubTbl *subTable;
    signed char areaIndex;
    int continuedKuppaScript;
    int playIntro;

    PathPtr::SetNodeTable(0);
    PathPtr::SetPathTable(0, 0);

    if (overlay->kclFileId != 0) {
        collisionFile = LoadFile(overlay->kclFileId);
        dBgW_Kc::UpdateFileOffsets(*collisionFile);
        ((dBgW_Kc *)meshCollider)->SetFile(collisionFile, *overlay->clps);
        func_0202a850(((dBgW_Kc *)meshCollider)->GetOctreeOriginY(),
                      ((dBgW_Kc *)meshCollider)->GetUnkOctreeY());
        ((dBgW *)(meshCollider))->Enable((dActor_c *)(0));
    }

    data_ov002_0211118c = 0;
    LoadObjects(*overlay->objTable, -1, entranceIndex);

    subTable = overlay->subTables;
    for (areaIndex = 0; areaIndex < overlay->subCount; areaIndex++) {
        if (subTable->table != 0)
            LoadObjects(*subTable->table, areaIndex, entranceIndex);
        subTable++;
    }

    continuedKuppaScript = ContinueKuppaScriptIfNecessary();

    playIntro = IntroAllowed() && continuedKuppaScript == 0;

    if (!IsGameMode(2)) {
        dBase_c::Spawn(0x14f, data_0209f5c0, 0, 0);
        if (playIntro == 0)
            dBase_c::Spawn(0x14e, data_0209f5c0, 0, 0);
    }

    if (playIntro != 0)
        StartIntroCutscene();
}
