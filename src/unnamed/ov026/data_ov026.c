#include "types.h"
#include "CLPS.h"
#include "private/BTA_File.h"

/* Definition order is what makes the compiler's size sort match address order. */

/* Widths the level loaders walk. Same layouts as LVL_Overlay; this file
 * stays C. Rotation is three s16s: Vector3s has a destructor. */
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

typedef struct DoorEntry {
    s16 x;
    s16 y;
    s16 z;
    s16 angle;
    u16 param;
    u16 type;
} DoorEntry;

/* ExitEntry is 0xe. The two bytes after it are zero, up to the next label. */
typedef struct ExitRow {
    s16 x;
    s16 y;
    s16 z;
    s16 rotX;
    s16 rotY;
    u8 param[4];
    u16 pad;
} ExitRow;

/* CamViewObj's 0xe bytes, plus those same two zero bytes. */
typedef struct ViewRow {
    u8 type;
    u8 p1;
    s16 x;
    s16 y;
    s16 z;
    u8 pad_8[2];
    s16 angleY;
    u8 tail[2];
    u16 pad;
} ViewRow;

/* LoadFog steps 8 bytes: flag, three colour bytes, near, far. */
typedef struct FogRow {
    u8 enabled;
    u8 r;
    u8 g;
    u8 b;
    u16 nearDist;
    u16 farDist;
} FogRow;

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

typedef struct SimpleEntry {
    u16 raw;
    s16 x;
    s16 y;
    s16 z;
} SimpleEntry;

typedef struct ClpsBlock {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entries[16];
    u32 tail[6];
} ClpsBlock;

typedef char ObjTable_size_must_be_0x8[sizeof(ObjTable) == 8 ? 1 : -1];
typedef char ObjSubTable_size_must_be_0x8[sizeof(ObjSubTable) == 8 ? 1 : -1];
typedef char DoorEntry_size_must_be_0xc[sizeof(DoorEntry) == 0xc ? 1 : -1];
typedef char ExitRow_size_must_be_0x10[sizeof(ExitRow) == 0x10 ? 1 : -1];
typedef char ViewRow_size_must_be_0x10[sizeof(ViewRow) == 0x10 ? 1 : -1];
typedef char FogRow_size_must_be_0x8[sizeof(FogRow) == 8 ? 1 : -1];
typedef char StandardEntry_size_must_be_0x10[sizeof(StandardEntry) == 0x10 ? 1 : -1];
typedef char SimpleEntry_size_must_be_0x8[sizeof(SimpleEntry) == 8 ? 1 : -1];
typedef char ClpsBlock_size_must_be_0xa0[sizeof(ClpsBlock) == 0xa0 ? 1 : -1];

/* Defined below. The BTA material rows and the area table name them. */
/* Interior of daObjWlKoopaShutter_c's .data. Not split out of that unit. */
extern u32 data_ov026_02112f94[];
extern ObjSubTable data_ov026_02113014[];
extern ClpsBlock data_ov026_02113100;
extern SimpleEntry data_ov026_02113450[];
extern StandardEntry data_ov026_021137c0[];

/* 0x02112494. BTA rotation key, one u16. The high half is zero. */
/* 0x02112498. Scale key, fx32 1.0. */

extern const u16 data_ov026_02112490[];
extern const fx32 data_ov026_021124a4[];
extern const BTAMaterialRecord data_ov026_021124e4[];
extern const fx32 data_ov026_02112554[];

typedef struct {
    void *objTable;
    BTA_File *bta;
    s8 minimapId;
    u8 pad[3];
} data_ov026_02112f10_Layout;
typedef struct {
    ClpsBlock *clps;
    ObjTable *objTable;
    u16 modelFileId;
    u16 kclFileId;
    u16 minimapTilesFile;
    u16 minimapPaletteFile;
    void *areas;
    u8 areaCount;
    u8 cameraFlags;
    s16 minimapScale;
    u32 skybox;
} data_ov026_02112f58_Layout;

extern const u16 data_ov026_02112494[2];
extern const fx32 data_ov026_02112498[1];
extern const fx32 data_ov026_0211249c[1];
extern const u16 data_ov026_021124a0[2];
extern BTAMaterialRecord data_ov026_021124ac[2];
extern BTAMaterialRecord data_ov026_0211251c[2];
extern fx32 data_ov026_021125cc[91];
extern fx32 data_ov026_02112738[273];
extern u16 data_ov026_02112dc0[2];
extern u32 data_ov026_02112dc4[1];
extern ObjTable data_ov026_02112dc8;
extern char data_ov026_02112dd0[8];
extern ObjTable data_ov026_02112dd8;
extern ObjTable data_ov026_02112de0;
extern char data_ov026_02112de8[12];
extern DoorEntry data_ov026_02112df4[1];
extern ExitRow data_ov026_02112e00[1];
extern ViewRow data_ov026_02112e10[1];
extern char data_ov026_02112e20[16];
extern char data_ov026_02112e30[16];
extern char data_ov026_02112e40[16];
extern char data_ov026_02112e50[16];
extern StandardEntry data_ov026_02112e60[1];
extern StandardEntry data_ov026_02112e70[1];
extern StandardEntry data_ov026_02112e80[1];
extern StandardEntry data_ov026_02112e90[1];
extern FogRow data_ov026_02112ea0[2];
extern StandardEntry data_ov026_02112eb0[1];
extern StandardEntry data_ov026_02112ec0[1];
extern StandardEntry data_ov026_02112ed0[1];
extern ObjSubTable data_ov026_02112ee0[3];
extern BTA_File data_ov026_02112ef8;
extern data_ov026_02112f10_Layout data_ov026_02112f10[2];
extern BTA_File data_ov026_02112f28;
extern BTA_File sWhirlpoolBta;
extern data_ov026_02112f58_Layout data_ov026_02112f58;

u16 data_ov026_02112dc0[2] = {0x07f4, 0x0000};

const u16 data_ov026_021124a0[2] = {0xfad8, 0x0000};

const fx32 data_ov026_0211249c[1] = {0x00001000};

const fx32 data_ov026_02112498[1] = {0x00001000};

u32 data_ov026_02112dc4[1] = {0x00000004};

const u16 data_ov026_02112494[2] = {0x0000, 0x0000};

ObjTable data_ov026_02112dd8 = {0x0003, {0x00, 0x00}, &data_ov026_02112ee0[0]};

ObjTable data_ov026_02112dc8 = {0x0009, {0x00, 0x00}, data_ov026_02113014};

char data_ov026_02112dd0[8] = "uzumaki";

ObjTable data_ov026_02112de0 = {0x0008, {0x00, 0x00}, data_ov026_02112f94};

DoorEntry data_ov026_02112df4[1] = {
    {(s16)0xff6a, 0x0320, 0x00fa, (s16)0xc000, 0x01ea, 0x0000},
};

char data_ov026_02112de8[12] = "uzumaki1";

char data_ov026_02112e30[16] = "mat_zy_water_1";

StandardEntry data_ov026_02112e90[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

StandardEntry data_ov026_02112e80[1] = {
    {0x0014, 0x28d2, 0x0546, 0x1004, 0x0003, (s16)0xc000, 0x0000, 0xffff},
};

StandardEntry data_ov026_02112e60[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

StandardEntry data_ov026_02112e70[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

FogRow data_ov026_02112ea0[2] = {
    {0x01, 0x57, 0x87, 0xba, 0x2af8, 0xb3b0},
    {0x01, 0x8c, 0xf7, 0xf7, 0x0000, 0xea60},
};

StandardEntry data_ov026_02112eb0[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

StandardEntry data_ov026_02112ec0[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

StandardEntry data_ov026_02112ed0[1] = {
    {0x0091, (s16)0xe2b6, (s16)0xfaf0, 0x06db, 0x0000, 0x0000, 0x0000, 0x7001},
};

ExitRow data_ov026_02112e00[1] = {
    {0x1de2, (s16)0xf9c0, (s16)0xe8c2, 0x0000, 0x0000, {0x01, 0x07, 0xff, 0xff}, 0x0000},
};

ViewRow data_ov026_02112e10[1] = {
    {0x02, 0xff, (s16)0xe412, 0x0b22, 0x073a, {0x00, 0x00}, (s16)0x8000, {0x00, 0x00}, 0x0000},
};

char data_ov026_02112e20[16] = "mat_zy_water_0";

char data_ov026_02112e40[16] = "mat_zz_light_0";

char data_ov026_02112e50[16] = "mat_zz_light_1";

data_ov026_02112f10_Layout data_ov026_02112f10[2] = {
    {&data_ov026_02112de0, &data_ov026_02112ef8, 0, {0x00, 0xae, 0x0c}},
    {&data_ov026_02112dd8, &data_ov026_02112f28, 0, {0x00, 0xa9, 0x0e}},
};

BTA_File data_ov026_02112f28 = {
    0x5b,
    0,
    (fx32 *)&data_ov026_0211249c[0],
    (u16 *)&data_ov026_021124a0[0],
    (fx32 *)&data_ov026_02112738[0],
    2,
    0,
    &data_ov026_0211251c[0],
};

BTA_File sWhirlpoolBta = {
    0x1e,
    0,
    (fx32 *)data_ov026_021124a4,
    (u16 *)data_ov026_02112490,
    (fx32 *)data_ov026_02112554,
    2,
    0,
    (BTAMaterialRecord *)data_ov026_021124e4,
};

ObjSubTable data_ov026_02112ee0[3] = {
    {0x00, 0x2d, {0x00, 0x00}, data_ov026_021137c0},
    {0x05, 0x36, {0x00, 0x00}, data_ov026_02113450},
    {0x60, 0x01, {0x00, 0x00}, &data_ov026_02112e80[0]},
};

BTA_File data_ov026_02112ef8 = {
    0x5b,
    0,
    (fx32 *)&data_ov026_02112498[0],
    (u16 *)&data_ov026_02112494[0],
    (fx32 *)&data_ov026_021125cc[0],
    2,
    0,
    &data_ov026_021124ac[0],
};

data_ov026_02112f58_Layout data_ov026_02112f58 = {
    &data_ov026_02113100,
    &data_ov026_02112dc8,
    0x07f5,
    0x07f3,
    0x07f6,
    0x07f7,
    &data_ov026_02112f10[0],
    2,
    0,
    0x0898,
    0x0000001f,
};

#pragma define_section s11251c ".rodata.data_ov026_0211251c"                                       \
                               ".rodata.data_ov026_0211251c"                                       \
                               ".rodata.data_ov026_0211251c" far_data
#pragma section s11251c begin
BTAMaterialRecord data_ov026_0211251c[2] = {
    {0xffff, 0, (char *)&data_ov026_02112e50[0], {1, 0}, {1, 0}, {1, 0}, {0x5b, 0}, {1, 0}},
    {0xffff,
     0,
     (char *)&data_ov026_02112e30[0],
     {1, 0},
     {1, 0},
     {1, 1},
     {0x5b, 0x5b},
     {0x5b, 0xb6}},
};

#pragma section s11251c end

#pragma define_section s1124ac ".rodata.data_ov026_021124ac"                                       \
                               ".rodata.data_ov026_021124ac"                                       \
                               ".rodata.data_ov026_021124ac" far_data
#pragma section s1124ac begin
BTAMaterialRecord data_ov026_021124ac[2] = {
    {0xffff, 0, (char *)&data_ov026_02112e40[0], {1, 0}, {1, 0}, {1, 0}, {0x5b, 0}, {0x5b, 0}},
    {0xffff, 0, (char *)&data_ov026_02112e20[0], {1, 0}, {1, 0}, {1, 0}, {0x5b, 0}, {0x5b, 0}},
};

#pragma section s1124ac end

#pragma define_section s1125cc ".rodata.data_ov026_021125cc"                                       \
                               ".rodata.data_ov026_021125cc"                                       \
                               ".rodata.data_ov026_021125cc" far_data
#pragma section s1125cc begin
fx32 data_ov026_021125cc[91] = {
    0x00000000, 0x0000002e, 0x0000005b, 0x00000089, 0x000000b6, 0x000000e4, 0x00000111, 0x0000013f,
    0x0000016c, 0x0000019a, 0x000001c7, 0x000001f5, 0x00000222, 0x00000250, 0x0000027d, 0x000002ab,
    0x000002d8, 0x00000306, 0x00000333, 0x00000361, 0x0000038e, 0x000003bc, 0x000003e9, 0x00000417,
    0x00000444, 0x00000472, 0x0000049f, 0x000004cd, 0x000004fa, 0x00000528, 0x00000555, 0x00000583,
    0x000005b0, 0x000005de, 0x0000060b, 0x00000639, 0x00000666, 0x00000694, 0x000006c1, 0x000006ef,
    0x0000071c, 0x0000074a, 0x00000777, 0x000007a5, 0x000007d2, 0x00000800, 0x0000082e, 0x0000085b,
    0x00000889, 0x000008b6, 0x000008e4, 0x00000911, 0x0000093f, 0x0000096c, 0x0000099a, 0x000009c7,
    0x000009f5, 0x00000a22, 0x00000a50, 0x00000a7d, 0x00000aab, 0x00000ad8, 0x00000b06, 0x00000b33,
    0x00000b61, 0x00000b8e, 0x00000bbc, 0x00000be9, 0x00000c17, 0x00000c44, 0x00000c72, 0x00000c9f,
    0x00000ccd, 0x00000cfa, 0x00000d28, 0x00000d55, 0x00000d83, 0x00000db0, 0x00000dde, 0x00000e0b,
    0x00000e39, 0x00000e66, 0x00000e94, 0x00000ec1, 0x00000eef, 0x00000f1c, 0x00000f4a, 0x00000f77,
    0x00000fa5, 0x00000fd2, 0x00001000};

fx32 data_ov026_02112738[273] = {
    0x00000000, 0xfffffffa, 0xfffffff2, 0xffffffea, 0xffffffe2, 0xffffffdb, 0xffffffd3, 0xffffffcc,
    0xffffffc6, 0xffffffc2, 0xffffffbf, 0xffffffbe, 0xffffffbf, 0xffffffc1, 0xffffffc5, 0xffffffca,
    0xffffffd1, 0xffffffd8, 0xffffffe1, 0xffffffeb, 0xfffffff6, 0x00000001, 0x0000000e, 0x0000001a,
    0x00000028, 0x00000036, 0x00000044, 0x00000053, 0x00000061, 0x00000070, 0x0000007f, 0x0000008e,
    0x0000009c, 0x000000ab, 0x000000b9, 0x000000c6, 0x000000d3, 0x000000df, 0x000000eb, 0x000000f6,
    0x000000ff, 0x00000108, 0x00000110, 0x00000116, 0x0000011c, 0x0000011f, 0x00000122, 0x00000123,
    0x00000123, 0x00000125, 0x00000128, 0x0000012b, 0x0000012f, 0x00000133, 0x00000136, 0x00000139,
    0x0000013a, 0x0000013a, 0x00000139, 0x00000136, 0x00000130, 0x00000129, 0x00000122, 0x0000011a,
    0x00000112, 0x00000109, 0x00000100, 0x000000f7, 0x000000ee, 0x000000e4, 0x000000da, 0x000000d0,
    0x000000c5, 0x000000bb, 0x000000b0, 0x000000a5, 0x0000009a, 0x0000008f, 0x00000083, 0x00000078,
    0x0000006d, 0x00000062, 0x00000056, 0x0000004b, 0x00000040, 0x00000035, 0x0000002a, 0x0000001f,
    0x00000015, 0x0000000a, 0x00000000, 0x00000000, 0x0000000d, 0x0000001a, 0x00000028, 0x00000036,
    0x00000045, 0x00000055, 0x00000064, 0x00000074, 0x00000085, 0x00000095, 0x000000a5, 0x000000b6,
    0x000000c6, 0x000000d7, 0x000000e7, 0x000000f7, 0x00000107, 0x00000116, 0x00000125, 0x00000134,
    0x00000142, 0x0000014f, 0x0000015c, 0x00000169, 0x00000174, 0x0000017f, 0x00000189, 0x00000192,
    0x00000199, 0x000001a0, 0x000001a6, 0x000001ab, 0x000001ae, 0x000001b0, 0x000001b1, 0x000001b1,
    0x000001b3, 0x000001b6, 0x000001b9, 0x000001bc, 0x000001c0, 0x000001c4, 0x000001c7, 0x000001ca,
    0x000001cb, 0x000001cc, 0x000001cc, 0x000001ca, 0x000001c8, 0x000001c5, 0x000001c1, 0x000001bc,
    0x000001b7, 0x000001b0, 0x000001a9, 0x000001a2, 0x0000019a, 0x00000191, 0x00000188, 0x0000017e,
    0x00000173, 0x00000169, 0x0000015d, 0x00000152, 0x00000146, 0x00000139, 0x0000012c, 0x00000120,
    0x00000112, 0x00000105, 0x000000f7, 0x000000ea, 0x000000dc, 0x000000ce, 0x000000c0, 0x000000b2,
    0x000000a4, 0x00000096, 0x00000089, 0x0000007b, 0x0000006d, 0x00000060, 0x00000053, 0x00000046,
    0x0000003a, 0x0000002d, 0x00000021, 0x00000016, 0x0000000b, 0x00000000, 0x00000000, 0x0000000a,
    0x00000012, 0x0000001a, 0x00000023, 0x00000030, 0x00000043, 0x00000058, 0x0000006d, 0x00000081,
    0x00000094, 0x000000a7, 0x000000b9, 0x000000ca, 0x000000db, 0x000000eb, 0x000000fb, 0x0000010a,
    0x00000118, 0x00000126, 0x00000133, 0x00000140, 0x0000014c, 0x00000157, 0x00000162, 0x0000016c,
    0x00000176, 0x0000017f, 0x00000188, 0x00000190, 0x00000197, 0x0000019f, 0x000001a5, 0x000001ab,
    0x000001b1, 0x000001b6, 0x000001bb, 0x000001bf, 0x000001c2, 0x000001c6, 0x000001c8, 0x000001cb,
    0x000001cd, 0x000001ce, 0x000001cf, 0x000001d0, 0x000001d0, 0x000001cf, 0x000001ce, 0x000001cd,
    0x000001cb, 0x000001c8, 0x000001c6, 0x000001c4, 0x000001c2, 0x000001be, 0x000001ba, 0x000001b4,
    0x000001ae, 0x000001a7, 0x0000019f, 0x00000196, 0x0000018c, 0x00000181, 0x00000176, 0x0000016b,
    0x0000015e, 0x00000151, 0x00000144, 0x00000136, 0x00000128, 0x0000011a, 0x0000010b, 0x000000fc,
    0x000000ec, 0x000000dd, 0x000000cd, 0x000000be, 0x000000ae, 0x0000009e, 0x0000008f, 0x0000007f,
    0x00000070, 0x00000061, 0x00000052, 0x00000043, 0x00000035, 0x00000027, 0x0000001a, 0x0000000c,
    0x00000000};

#pragma section s1125cc end
