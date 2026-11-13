//cpp
/* leftovers:
 * - Source order stays highest address first. Reordering moves the run.
 * - sObjectLoaders and the small loader tables stay data. This TU
 *   claims .text only.
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
#include "KuppaScript.h"
#include "decl_common.h"

#include "LVL_Overlay.h"
#include "dScStage_c.h"
#include "dActor_c.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "dBgW.h"
#include "PathPtr.h"
#include "Vector3s.h"
#include "SaveData.h"

/* The old per-loader files disagreed on four externs. One TU can keep
 * one spelling of each. */
extern "C" {

/* data_0209caa0 is the live save block; its first 0x44 bytes are the
 * current file. LoadEntranceObjects reads the character, LoadClsnAndObjects
 * flags2. */

/* gCurrentScene, the running scene, is the parent passed to dBase_c::Spawn. */
extern fBase_c *gCurrentScene;

extern u8 gNumPlayers;
extern u8 gCurrentStarId;
extern u8 gLocalPlayerId;
extern u8 gGameMode;
extern dCamera_c *gCamera;
extern void *gUnusedType13Objs;
extern daPly_c *gPlayers[];
extern u8 data_0209fc5c[];
extern unsigned int data_02092134;
extern u8 data_02092128[];

/* The loaders' lookup tables, all ov002 .data. sEntranceWipes is the
 * fader wipe per entrance kind (negative: none). sDoorVariants and
 * sDoorActorIds are indexed by door type: the variant daDoor_c and
 * daStarGate_c read from param bits 16..23, and the actor id (CH_ROOM,
 * DOOR or STAR_GATE). sObjToActorIdTable maps a level object id to its
 * actor id. */
extern s8 sEntranceWipes[];
extern u8 sDoorVariants[];
extern u16 sDoorActorIds[];
extern u16 sObjToActorIdTable[];

/* The death-table ID the next spawned object gets (the SM64DS modding symbol
 * list's NEXT_DEATH_TABLE_ID): LoadClsnAndObjects zeroes it, and every
 * loader passes it, post-incremented, as dActor_c::Spawn's deathTableID. */
extern s16 sNextDeathTableID;
}

void func_0202af80(void *entries, s32 count);
void func_0202b044(void *entries, s32 count);
void func_0202b060(void *entries, s32 count);
void func_0202b090(void *entries, s32 count);
void func_0202b0c4(void *entries, s32 count);
void func_0202b0e0(LVL_Overlay::StandardEntry *e, int count);
void StartEntranceFaderWipe(int index);
extern "C" {

/* dScStage_c::LoadClsnAndObjects' own callees. LoadFile is not in decl_common.h. */
struct KCL_File;
void *LoadFile(int handle);
}

/* LoadObjects dispatches through this table, indexed by object category.
 * Its entries are the fifteen category loaders. This text-only TU does not
 * define it. */
typedef void (*ObjectLoader)(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param);
extern "C" {

extern ObjectLoader sObjectLoaders[];
}
/* extern "C" */

/* The loaders test the game mode as a 0/1 value. */
static inline int IsGameMode(int mode) {
    return gGameMode == mode;
}

/* Game mode 0 with bit 0x80 of the current file's flags2 clear. */
static inline int IntroAllowed() {
    return IsGameMode(0) && (data_0209caa0.file.flags2 & FILE_FLAGS2_INTRO_DONE) == 0;
}

/* Spawn simple actors, or hand a minimap-change sentinel to
 * LoadMinimapChangeObject. Each SimpleEntry is 8 bytes: a packed `raw` word
 * (low 9 bits = actor-table index, high bits = param) and an s16 position.
 * There is no rotation; spawn passes NULL for it. raw == 0x1ff is the
 * minimap-change special case. The third parameter is part of the shared loader
 * signature and unused here. */
void LoadSimpleObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::SimpleEntry *entry = (LVL_Overlay::SimpleEntry *)table.entries;
    for (int objectIndex = 0; objectIndex < table.count; objectIndex++, entry++) {
        Vector3 position(entry->x << 12, entry->y << 12, entry->z << 12);
        u16 actorIndex = entry->raw & 0x1ff;
        int actorParam = entry->raw >> 9;
        if (actorIndex == 0x1ff) {
            LoadMinimapChangeObject(areaID, position.y, actorParam & 0xf);
        } else {
            dActor_c::Spawn(sObjToActorIdTable[actorIndex], actorParam, position, 0, areaID,
                            sNextDeathTableID++);
        }
    }
}

/* One actor per standard object entry. Each StandardEntry is 0x10 bytes: raw id
 * index into sObjToActorIdTable, s16 position (shifted left by 12 into Fix12),
 * Vector3s rotation, u16 param. Each spawn takes the next death-table ID. The
 * third parameter is part of the shared loader signature and unused here. */
void LoadStandardObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::StandardEntry *entry = (LVL_Overlay::StandardEntry *)table.entries;
    for (int objectIndex = 0; objectIndex < table.count; objectIndex++, entry++) {
        u32 actorID = sObjToActorIdTable[entry->raw];
        Vector3 position(entry->x << 12, entry->y << 12, entry->z << 12);
        dActor_c::Spawn(actorID, entry->param, position, (const Vector3_16 *)&entry->rot, areaID,
                        sNextDeathTableID++);
    }
}

/* Spawn entrance actors and the entrance-controller dBase_c. Walks
 * StandardEntry records like LoadStandardObjects, but the third argument is an
 * entry index OFFSET into the table rather than a free parameter: the walk
 * starts at `entries + entranceIndex` and runs for gNumPlayers iterations, not table.count. */
void LoadEntranceObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 entranceIndex) {
    u32 entranceKind; /* declared first: at its first assignment it is 20 words off */
    LVL_Overlay::StandardEntry *entry = (LVL_Overlay::StandardEntry *)table.entries;
    func_0202b0e0(entry, table.count);
    entry += entranceIndex;
    entranceKind = 0;
    int entranceId = 0xf;

    for (u8 playerIndex = 0; playerIndex < gNumPlayers; entry++, playerIndex++) {
        Vector3 pos(entry->x << 12, entry->y << 12, entry->z << 12);

        u16 param = entry->param;
        entranceKind = (param >> 7) & 0xf;

        int savedCharacter = data_0209caa0.file.mCharacter;
        int playerCharacter = data_02092128[playerIndex];
        if (IsGameMode(1)) {
            savedCharacter = 3;
            playerCharacter = 3;
        }
        u32 spawnParam =
            savedCharacter | (playerCharacter << 3) | (playerIndex << 6) | (entranceKind << 8);

        daPly_c *player =
            (daPly_c *)dActor_c::Spawn(sObjToActorIdTable[entry->raw], spawnParam, pos,
                                       (const Vector3_16 *)&entry->rot, param & 7, -1);

        if (data_0209fc5c[playerIndex] == 0)
            gPlayers[playerIndex] = 0;
        else
            gPlayers[playerIndex] = player;

        if (playerIndex == gLocalPlayerId)
            entranceId = (entry->param >> 3) & 0xf;
    }

    gCamera = (dCamera_c *)dBase_c::Spawn(0x14c, gCurrentScene, entranceId, 0);

    if (IsGameMode(2))
        return;

    if (entranceKind >= 0x13)
        entranceKind = 0;
    int wipeType = sEntranceWipes[entranceKind];
    if (wipeType < 0)
        return;

    StartEntranceFaderWipe(wipeType);
}

void LoadPathNodeObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    PathPtr::SetNodeTable(table.entries);
}

void LoadPathObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    PathPtr::SetPathTable((PathDef *)table.entries, table.count);
}

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
void LoadTeleportSourceObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::TeleportSourceEntry *entry = (LVL_Overlay::TeleportSourceEntry *)table.entries;

    s32 objectIndex = 0;
    if ((int)table.count <= 0) /* signed ble; without the cast it is 3 words off */
        return;

    do {
        Vector3 pos(entry->x << 12, entry->y << 12, entry->z << 12);

        dActor_c::Spawn(0x15b, entry->param, pos, 0, 0, -1);

        objectIndex++;
        entry++;
    } while (objectIndex < table.count);
}

void LoadTeleportDestObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b090(table.entries, table.count);
}

void LoadFogObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b060(table.entries, table.count);
}

/* One door actor per entry. The actor id and the high half of its parameter come
 * out of two small ov002 tables indexed by the low five bits of the record's
 * last halfword; the rotation is unpacked from two nibbles of param plus the
 * stored angle. */
void LoadDoorObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::DoorEntry *entry = (LVL_Overlay::DoorEntry *)table.entries;
    for (int objectIndex = 0; objectIndex < table.count; objectIndex++, entry++) {
        Vector3 pos(entry->x << 12, entry->y << 12, entry->z << 12);
        /* All three angles are computed before any is stored: member stores or
           an aggregate initializer are +4 bytes, a u16 param copy 11 words off. */
        Vector3_16 rot;
        s16 rx = (entry->param >> 8) & 0xf;
        s16 ry = entry->angle;
        s16 rz = (entry->param >> 12) & 0xf;
        rot.x = rx;
        rot.y = ry;
        rot.z = rz;
        int doorType = entry->type & 0x1f;
        u32 actorID = sDoorActorIds[doorType];
        u32 spawnParam = entry->param | (sDoorVariants[doorType] << 16);
        dActor_c::Spawn(actorID, spawnParam, pos, &rot, -1, -1);
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
void LoadExitObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    LVL_Overlay::ExitEntry *entry = (LVL_Overlay::ExitEntry *)table.entries;

    for (int objectIndex = 0; objectIndex < table.count; objectIndex++, entry++) {
        Vector3 pos(entry->x << 12, entry->y << 12, entry->z << 12);

        Vector3s rot;
        rot.x = -entry->rotX;
        rot.y = -entry->rotY;

        dActor_c::Spawn(0x15d,
                        ((u32)entry->param[0] << 24) | ((u32)entry->param[1] << 16) |
                            ((u32)entry->param[2] << 8) | (u32)entry->param[3],
                        pos, (const Vector3_16 *)&rot, -1, -1);
    }
}

void LoadMinimapTileObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202b044(table.entries, table.count);
}

void LoadMinimapScaleObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    func_0202af80(table.entries, table.count);
}

/* Stash the entry array's ADDRESS and do nothing with it. Not a mistake in the
 * recovery: the ROM really does store the pointer rather than walk it. Object
 * type 13 is unused in the shipped game, so the loader was reduced to a stub
 * that keeps the table around, which also explains why it reads no count. */
void LoadUnusedType13Objects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    gUnusedType13Objs = table.entries;
}

/* Take the first word of the star-camera table and publish it. Reads no count,
 * unlike most of this family: it wants one value, not a list. */
void LoadStarCameraObjects(LVL_Overlay::ObjSubTable &table, s8 areaID, u32 param) {
    data_02092134 = *(unsigned int *)table.entries;
}

/* Walk the outer object table and dispatch each sub-table to the category
 * loader table at sObjectLoaders -- whose fifteen entries are exactly the
 * fifteen functions above.
 *
 * Count is a u16 at offset 0 (unlike ObjSubTable's u8 at offset 1). Each entry
 * is the 8-byte ObjSubTable its loader takes; its byte 0 packs a star filter in
 * the top three bits and a handler index in the low five. The entry is skipped
 * unless the filter is 0 or equal to the selected star at gCurrentStarId. */
void LoadObjects(LVL_Overlay::ObjTable &table, s8 areaID, u32 param) {
    LVL_Overlay::ObjSubTable *entry = (LVL_Overlay::ObjSubTable *)table.entries;
    for (int tableIndex = 0; tableIndex < table.count; tableIndex++) {
        u8 categoryAndStar = entry->categoryAndStar;
        int starFilter = (categoryAndStar >> 5) & 7;
        if (starFilter == 0 || starFilter == gCurrentStarId) {
            ObjectLoader loader = sObjectLoaders[categoryAndStar & 0x1f];
            if (loader != 0)
                loader(*entry, areaID, param);
        }
        entry++;
    }
}

/* Loads the collision mesh, runs the object tables, then decides
 * whether the intro cutscene plays. LoadObjects is in this TU, so the
 * call is by name. */
void dScStage_c::LoadClsnAndObjects(LVL_Overlay &overlay, u32 entranceIndex,
                                    dBgW_Kc &meshCollider) {
    PathPtr::SetNodeTable(0);
    PathPtr::SetPathTable(0, 0);

    if (overlay.kclFileId != 0) {
        KCL_File *collisionFile = static_cast<KCL_File *>(LoadFile(overlay.kclFileId));
        dBgW_Kc::UpdateFileOffsets(*collisionFile);
        meshCollider.SetFile(collisionFile, *overlay.clps);
        func_0202a850(meshCollider.GetOctreeOriginY(), meshCollider.GetUnkOctreeY());
        meshCollider.Enable(0);
    }

    sNextDeathTableID = 0;
    LoadObjects(*overlay.objTable, -1, entranceIndex);

    LVL_Overlay::Area *area = overlay.areas;
    for (s8 areaIndex = 0; areaIndex < overlay.areaCount; areaIndex++, area++) {
        if (area->objTable != 0)
            LoadObjects(*area->objTable, areaIndex, entranceIndex);
    }

    int continuedKuppaScript = KuppaScript::ContinueIfNecessary();
    bool playIntro = IntroAllowed() && continuedKuppaScript == 0;

    if (!IsGameMode(2)) {
        dBase_c::Spawn(0x14f, gCurrentScene, 0, 0);
        if (!playIntro)
            dBase_c::Spawn(0x14e, gCurrentScene, 0, 0);
    }

    if (playIntro)
        KuppaScript::StartIntroCutscene();
}
