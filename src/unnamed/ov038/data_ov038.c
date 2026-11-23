#include "CLPS.h"
#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

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
    u16 raw;
    s16 x;
    s16 y;
    s16 z;
    s16 rotX;
    s16 rotY;
    s16 rotZ;
    u16 param;
} StandardEntry;

typedef struct ExitEntry {
    s16 x;
    s16 y;
    s16 z;
    s16 rotX;
    s16 rotY;
    u8 param[4];
} ExitEntry;

typedef struct ViewEntry {
    u8 type;
    u8 p1;
    s16 x;
    s16 y;
    s16 z;
    u8 pad_8[2];
    s16 angleY;
    u16 pad_c;
} ViewEntry;

typedef struct FogEntry {
    u8 enabled;
    u8 r;
    u8 g;
    u8 b;
    u16 nearDist;
    u16 farDist;
} FogEntry;

typedef struct Area {
    struct ObjTable *objTable;
    void *bta;
    s8 minimapId;
    u8 pad[3];
} Area;

typedef struct LevelRec {
    void *clps;
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

typedef char ObjTable_size[sizeof(ObjTable) == 8 ? 1 : -1];
typedef char ObjSubTable_size[sizeof(ObjSubTable) == 8 ? 1 : -1];
typedef char SimpleEntry_size[sizeof(SimpleEntry) == 8 ? 1 : -1];
typedef char StandardEntry_size[sizeof(StandardEntry) == 0x10 ? 1 : -1];
typedef char ExitEntry_size[sizeof(ExitEntry) == 0xe ? 1 : -1];
typedef char ViewEntry_size[sizeof(ViewEntry) == 0xe ? 1 : -1];
typedef char FogEntry_size[sizeof(FogEntry) == 8 ? 1 : -1];
typedef char Area_size[sizeof(Area) == 0xc ? 1 : -1];
typedef char LevelRec_size[sizeof(LevelRec) == 0x1c ? 1 : -1];

typedef struct {
    ViewEntry entry;
    u16 pad;
} data_ov038_021111e8_Layout;
typedef struct {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entries[2];
    u32 tail[2];
} data_ov038_02111254_Layout;
typedef struct {
    SimpleEntry entries[65];
    u32 tail[5];
} data_ov038_021112e4_Layout;

extern u32 data_ov038_021111c0;
extern u32 data_ov038_021111c4;
extern u32 data_ov038_021111c8;
extern ObjTable data_ov038_021111cc;
extern ObjTable data_ov038_021111d4;
extern Area data_ov038_021111dc;
extern data_ov038_021111e8_Layout data_ov038_021111e8;
extern FogEntry data_ov038_021111f8[2];
extern ObjSubTable data_ov038_02111208[2];
extern LevelRec data_ov038_02111218;
extern StandardEntry data_ov038_02111234[2];
extern data_ov038_02111254_Layout data_ov038_02111254;
extern ObjSubTable data_ov038_02111274[6];
extern StandardEntry data_ov038_021112a4[4];
extern data_ov038_021112e4_Layout data_ov038_021112e4;

u32 data_ov038_021111c0 = 0x00000c80;

u32 data_ov038_021111c4 = 0x000007c8;

u32 data_ov038_021111c8 = 0x00000004;

ObjTable data_ov038_021111cc = {0x0002, {0x00, 0x00}, &data_ov038_02111208[0]};

ObjTable data_ov038_021111d4 = {0x0006, {0x00, 0x00}, &data_ov038_02111274[0]};

Area data_ov038_021111dc = {&data_ov038_021111cc, 0, 0x00, {0x00, 0x0a, 0x00}};

FogEntry data_ov038_021111f8[2] = {
    {0x00, 0xff, 0xff, 0xff, 0x2710, 0x4e20},
    {0x01, 0x00, 0x70, 0xc0, 0x0000, 0x9470},
};

ObjSubTable data_ov038_02111208[2] = {
    {0x00, 0x02, {0x00, 0x00}, &data_ov038_02111234[0]},
    {0x05, 0x41, {0x00, 0x00}, &data_ov038_021112e4},
};

data_ov038_021111e8_Layout data_ov038_021111e8 = {
    {0x02, 0xff, 0x0000, (s16)0xfed4, 0x0000, {0x00, 0x00}, 0x0000, 0x0000}, 0x0000};

LevelRec data_ov038_02111218 = {&data_ov038_02111254,
                                &data_ov038_021111d4,
                                0x07c9,
                                0x07c7,
                                0x07ca,
                                0x07cb,
                                &data_ov038_021111dc,
                                0x01,
                                0x00,
                                0x0320,
                                0x0000002f};

StandardEntry data_ov038_02111234[2] = {
    {0x0031, 0x0000, (s16)0xf1f0, 0x0000, 0x0000, 0x0000, 0x0000, 0x00f8},
    {0x0031, 0x0000, (s16)0xf1f0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0017},
};

data_ov038_02111254_Layout data_ov038_02111254 = {
    0x53504c43,
    0x0008,
    0x0002,
    {
        {0x00040fc0, 0x000000ff},
        {0x00040fe0, 0x000000ff},
    },
    {0xffffffff, 0xffffffff},
};

ObjSubTable data_ov038_02111274[6] = {
    {0x04, 0x01, {0x00, 0x00}, &data_ov038_021111e8},
    {0x0b, 0x01, {0x00, 0x00}, &data_ov038_021111c4},
    {0x01, 0x04, {0x00, 0x00}, &data_ov038_021112a4[0]},
    {0x08, 0x02, {0x00, 0x00}, &data_ov038_021111f8[0]},
    {0x0c, 0x01, {0x00, 0x00}, &data_ov038_021111c0},
    {0x0e, 0x01, {0x00, 0x00}, &data_ov038_021111c8},
};

StandardEntry data_ov038_021112a4[4] = {
    {0x0000, 0x0000, (s16)0xfd44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080},
    {0x0000, (s16)0xfe0c, (s16)0xfd44, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080},
    {0x0000, (s16)0xff38, (s16)0xfd44, (s16)0xfe70, 0x0000, 0x0000, 0x0000, 0x0080},
    {0x0000, 0x015e, (s16)0xfd44, (s16)0xfe70, 0x0000, 0x0000, 0x0000, 0x0080},
};

data_ov038_021112e4_Layout data_ov038_021112e4 = {
    {
        {0x2226, (s16)0xf6a0, (s16)0xee6c, (s16)0xf6a0},
        {0x2226, (s16)0xf6a0, (s16)0xee6c, 0x08fc},
        {0x2226, 0x0960, (s16)0xee6c, 0x08fc},
        {0x2226, 0x0960, (s16)0xee6c, (s16)0xf6a0},
        {0x023f, 0x0000, (s16)0xef66, 0x0000},
        {0xe225, 0x0000, (s16)0xf1ed, 0x01f4},
        {0xe225, 0x0000, (s16)0xf1ed, (s16)0xfe0c},
        {0xe225, (s16)0xfe0c, (s16)0xf1ed, 0x0000},
        {0xe225, 0x01f4, (s16)0xf1ed, 0x0000},
        {0xe225, 0x015e, (s16)0xf1ed, 0x015e},
        {0xe225, (s16)0xfea2, (s16)0xf1ed, 0x015e},
        {0xe225, (s16)0xfea2, (s16)0xf1ed, (s16)0xfea2},
        {0xe225, 0x015e, (s16)0xf1ed, (s16)0xfea2},
        {0x2226, 0x08fc, (s16)0xf7cd, 0x0000},
        {0x2226, (s16)0xf704, (s16)0xf7cc, 0x0000},
        {0x2226, 0x0000, (s16)0xf7c3, 0x0898},
        {0x2226, 0x0000, (s16)0xf7cc, (s16)0xf704},
        {0xe225, 0x0000, (s16)0xf6e6, (s16)0xf7cc},
        {0xe225, 0x0000, (s16)0xf6e6, (s16)0xf63c},
        {0xe225, 0x0000, (s16)0xf8f8, (s16)0xf704},
        {0xe225, 0x0000, (s16)0xf6a0, (s16)0xf704},
        {0xe225, 0x0000, (s16)0xf7cc, (s16)0xf830},
        {0xe225, 0x0000, (s16)0xf8b3, (s16)0xf7cc},
        {0xe225, 0x0000, (s16)0xf8bb, (s16)0xf63c},
        {0xe225, 0x0000, (s16)0xf7cc, (s16)0xf5d8},
        {0xe225, 0x0000, (s16)0xf6d3, 0x0960},
        {0xe225, 0x0000, (s16)0xf6d3, 0x07d0},
        {0xe225, 0x0000, (s16)0xf8e6, 0x0898},
        {0xe225, 0x0000, (s16)0xf68e, 0x0898},
        {0xe225, 0x0000, (s16)0xf7ba, 0x09c4},
        {0xe225, 0x0000, (s16)0xf8a0, 0x0960},
        {0xe225, 0x0000, (s16)0xf8a8, 0x07d0},
        {0xe225, 0x0000, (s16)0xf7ba, 0x076c},
        {0xe225, (s16)0xf830, (s16)0xf7cc, 0x0000},
        {0xe225, (s16)0xf5d8, (s16)0xf7cc, 0x0000},
        {0xe225, (s16)0xf704, (s16)0xf8f8, 0x0000},
        {0xe225, (s16)0xf704, (s16)0xf6a0, 0x0000},
        {0xe225, (s16)0xf63c, (s16)0xf6e3, 0x0000},
        {0xe225, (s16)0xf7cc, (s16)0xf6de, 0x0000},
        {0xe225, (s16)0xf7cc, (s16)0xf8b6, 0x0000},
        {0xe225, (s16)0xf63c, (s16)0xf8ad, 0x0000},
        {0xe225, 0x0a28, (s16)0xf7cd, 0x0000},
        {0xe225, 0x07d0, (s16)0xf7cd, 0x0000},
        {0xe225, 0x08fc, (s16)0xf8f8, 0x0000},
        {0xe225, 0x08fc, (s16)0xf6a1, 0x0000},
        {0xe225, 0x0834, (s16)0xf6e3, 0x0000},
        {0xe225, 0x09c4, (s16)0xf6de, 0x0000},
        {0xe225, 0x09c4, (s16)0xf8b6, 0x0000},
        {0xe225, 0x0834, (s16)0xf8ad, 0x0000},
        {0x0a36, 0x0553, (s16)0xf0c7, 0x02e0},
        {0x2a36, 0x04c2, (s16)0xf299, 0x03b5},
        {0x0a36, 0x04f9, (s16)0xf398, (s16)0xfa7a},
        {0x4a36, 0x0468, (s16)0xf57f, (s16)0xfb65},
        {0x0a36, (s16)0xf99e, (s16)0xf0d2, (s16)0xfbfe},
        {0x0a36, (s16)0xf90d, (s16)0xf28f, (s16)0xfc9f},
        {0x2a36, (s16)0xfbc7, (s16)0xf0a7, 0x062b},
        {0x4a36, (s16)0xfb36, (s16)0xf265, 0x0700},
        {0x0a36, 0x0553, (s16)0xf9ce, 0x02ed},
        {0x2a36, 0x04c2, (s16)0xfbbc, 0x00bc},
        {0x2a36, (s16)0xfbc7, (s16)0xfab1, (s16)0xfa39},
        {0x4a36, (s16)0xfb36, (s16)0xfc6e, (s16)0xfb0e},
        {0x0a36, 0x04f9, (s16)0xf844, 0x0687},
        {0x4a36, 0x0468, (s16)0xfa2c, 0x0772},
        {0x2a36, (s16)0xfbc7, (s16)0xf774, 0x00f6},
        {0x4a36, (s16)0xfb36, (s16)0xf9b9, (s16)0xfed4},
    },
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
};
