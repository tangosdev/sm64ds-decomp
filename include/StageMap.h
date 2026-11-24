#ifndef STAGE_MAP_H
#define STAGE_MAP_H

/* C layouts of the stage-map records. The C++ names live as nested types in
 * LVL_Overlay.h; these are the same widths, for the stage-map translation
 * units, which are C. */
#include "types.h"

struct CLPS_Block;

typedef struct ObjTable {
    u16 count;
    u8 pad[2];
    void *entries;
} ObjTable;

typedef struct ObjSubTable {
    u8 categoryAndStar;
    u8 count;
    u8 pad[2];
    void *entries;
} ObjSubTable;

typedef struct SimpleEntry {
    u16 raw;
    s16 x;
    s16 y;
    s16 z;
} SimpleEntry;

typedef struct StandardEntry {
    u16 raw;   /* 0x00 */
    s16 x;     /* 0x02 */
    s16 y;     /* 0x04 */
    s16 z;     /* 0x06 */
    s16 rotX;  /* 0x08 */
    s16 rotY;  /* 0x0a */
    s16 rotZ;  /* 0x0c */
    u16 param; /* 0x0e */
} StandardEntry;

typedef struct PathNode {
    s16 x; /* 0x00 */
    s16 y; /* 0x02 */
    s16 z; /* 0x04 */
} PathNode;

typedef struct DoorEntry {
    s16 x;
    s16 y;
    s16 z;
    s16 angle;
    u16 param;
    u16 type;
} DoorEntry;

typedef struct ExitEntry {
    s16 x;
    s16 y;
    s16 z;
    s16 rotX;
    s16 rotY;
    u8 param[4];
} ExitEntry;

typedef struct TeleportSourceEntry {
    s16 x;
    s16 y;
    s16 z;
    u16 param;
} TeleportSourceEntry;

typedef struct TeleportDest {
    s32 a;
    s32 b;
} TeleportDest;

/* 0xe. SetViewObjs steps this far. */
typedef struct ViewEntry {
    u8 type;
    u8 p1;
    s16 x;
    s16 y;
    s16 z;
    u8 pad[2];
    s16 angleY;
    u16 pad_c;
} ViewEntry;

typedef struct Area {
    struct ObjTable *objTable;
    void *bta;
    s8 minimapId;
    u8 pad[3];
} Area;

/* The level record dScStage_c::LoadClsnAndObjects reads. 0x1c. */
typedef struct LevelRec {
    struct CLPS_Block *clps;
    struct ObjTable *objTable;
    u16 modelFileId;
    u16 kclFileId;
    u16 minimapTilesFile;
    u16 minimapPaletteFile;
    struct Area *areas;
    u8 areaCount;
    u8 cameraFlags;
    s16 minimapScale;
    u32 skybox;
} LevelRec;

typedef char ObjTable_size_must_be_8[sizeof(ObjTable) == 8 ? 1 : -1];
typedef char ObjSubTable_size_must_be_8[sizeof(ObjSubTable) == 8 ? 1 : -1];
typedef char SimpleEntry_size_must_be_8[sizeof(SimpleEntry) == 8 ? 1 : -1];
typedef char StandardEntry_size_must_be_0x10[sizeof(StandardEntry) == 0x10 ? 1 : -1];
typedef char DoorEntry_size_must_be_0xc[sizeof(DoorEntry) == 0xc ? 1 : -1];
typedef char ExitEntry_size_must_be_0xe[sizeof(ExitEntry) == 0xe ? 1 : -1];
typedef char TeleportSourceEntry_size_must_be_8[sizeof(TeleportSourceEntry) == 8 ? 1 : -1];
typedef char TeleportDest_size_must_be_8[sizeof(TeleportDest) == 8 ? 1 : -1];
typedef char PathNode_size_must_be_6[sizeof(PathNode) == 6 ? 1 : -1];
typedef char ViewEntry_size_must_be_0xe[sizeof(ViewEntry) == 0xe ? 1 : -1];
typedef char Area_size_must_be_0xc[sizeof(Area) == 0xc ? 1 : -1];
typedef char LevelRec_size_must_be_0x1c[sizeof(LevelRec) == 0x1c ? 1 : -1];

#endif
