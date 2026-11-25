#ifndef STAGE_MAP_H
#define STAGE_MAP_H

/* C layouts of the stage-map records. The C++ names live as nested types in
 * LVL_Overlay.h; these are the same widths, for the stage-map translation
 * units, which are C. */
#include "types.h"

struct CLPS_Block;

typedef struct ObjTable {
    u16 count;     /* 0x00 */
    u8 pad[2];     /* 0x02 */
    void *entries; /* 0x04 */
} ObjTable;

typedef struct ObjSubTable {
    u8 categoryAndStar; /* 0x00 */
    u8 count;           /* 0x01 */
    u8 pad[2];          /* 0x02 */
    void *entries;      /* 0x04 */
} ObjSubTable;

typedef struct SimpleEntry {
    u16 raw; /* 0x00 */
    s16 x;   /* 0x02 */
    s16 y;   /* 0x04 */
    s16 z;   /* 0x06 */
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
    s16 x;     /* 0x00 */
    s16 y;     /* 0x02 */
    s16 z;     /* 0x04 */
    s16 angle; /* 0x06 */
    u16 param; /* 0x08 */
    u16 type;  /* 0x0a */
} DoorEntry;

typedef struct ExitEntry {
    s16 x;       /* 0x00 */
    s16 y;       /* 0x02 */
    s16 z;       /* 0x04 */
    s16 rotX;    /* 0x06 */
    s16 rotY;    /* 0x08 */
    u8 param[4]; /* 0x0a */
} ExitEntry;

typedef struct TeleportSourceEntry {
    s16 x;     /* 0x00 */
    s16 y;     /* 0x02 */
    s16 z;     /* 0x04 */
    u16 param; /* 0x06 */
} TeleportSourceEntry;

/* Destination row the player reads: x/y/z shifted up by 12, param's low
 * nibble the area, the next nibble the yaw. */
typedef struct TeleportDest {
    s16 x;     /* 0x00 */
    s16 y;     /* 0x02 */
    s16 z;     /* 0x04 */
    u16 param; /* 0x06 */
} TeleportDest;

/* One 8-byte fog record. The loader packs color[0..2] into the low, middle
 * and high five bits of the hardware color and passes the two halfwords to
 * Fog::Init as the near/far pair. */
typedef struct FogEntry {
    u8 enabled;  /* 0x00 */
    u8 color[3]; /* 0x01 */
    u16 near;    /* 0x04 */
    u16 far;     /* 0x06 */
} FogEntry;

/* 0xe. SetViewObjs steps this far. */
typedef struct ViewEntry {
    u8 type;    /* 0x00 */
    u8 p1;      /* 0x01 */
    s16 x;      /* 0x02 */
    s16 y;      /* 0x04 */
    s16 z;      /* 0x06 */
    u8 pad[2];  /* 0x08 */
    s16 angleY; /* 0x0a */
    u16 pad_c;  /* 0x0c */
} ViewEntry;

typedef struct Area {
    struct ObjTable *objTable; /* 0x00 */
    void *bta;                 /* 0x04 */
    s8 minimapId;              /* 0x08 */
    u8 pad[3];                 /* 0x09 */
} Area;

/* The level record dScStage_c::LoadClsnAndObjects reads. 0x1c. */
typedef struct LevelRec {
    struct CLPS_Block *clps;   /* 0x00 */
    struct ObjTable *objTable; /* 0x04 */
    u16 modelFileId;           /* 0x08 */
    u16 kclFileId;             /* 0x0a */
    u16 minimapTilesFile;      /* 0x0c */
    u16 minimapPaletteFile;    /* 0x0e */
    struct Area *areas;        /* 0x10 */
    u8 areaCount;              /* 0x14 */
    u8 cameraFlags;            /* 0x15 */
    s16 minimapScale;          /* 0x16 */
    u32 skybox;                /* 0x18 */
} LevelRec;

typedef char ObjTable_size_must_be_8[sizeof(ObjTable) == 8 ? 1 : -1];
typedef char ObjSubTable_size_must_be_8[sizeof(ObjSubTable) == 8 ? 1 : -1];
typedef char SimpleEntry_size_must_be_8[sizeof(SimpleEntry) == 8 ? 1 : -1];
typedef char StandardEntry_size_must_be_0x10[sizeof(StandardEntry) == 0x10 ? 1 : -1];
typedef char DoorEntry_size_must_be_0xc[sizeof(DoorEntry) == 0xc ? 1 : -1];
typedef char ExitEntry_size_must_be_0xe[sizeof(ExitEntry) == 0xe ? 1 : -1];
typedef char TeleportSourceEntry_size_must_be_8[sizeof(TeleportSourceEntry) == 8 ? 1 : -1];
typedef char TeleportDest_size_must_be_8[sizeof(TeleportDest) == 8 ? 1 : -1];
typedef char FogEntry_size_must_be_8[sizeof(FogEntry) == 8 ? 1 : -1];
typedef char PathNode_size_must_be_6[sizeof(PathNode) == 6 ? 1 : -1];
typedef char ViewEntry_size_must_be_0xe[sizeof(ViewEntry) == 0xe ? 1 : -1];
typedef char Area_size_must_be_0xc[sizeof(Area) == 0xc ? 1 : -1];
typedef char LevelRec_size_must_be_0x1c[sizeof(LevelRec) == 0x1c ? 1 : -1];

#endif
