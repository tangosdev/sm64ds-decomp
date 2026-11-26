#include "CLPS.h"
#include "PathPtr.h"
#include "StageMap.h"
#include "private/BTA_File.h"
#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 tag;
    u16 stride;
    u16 count;
    struct CLPS entries[3];
} Clps_021119b0;

/* The 6-byte table is followed by two zero bytes before the
 * next object, so the padding is written as part of the object. */
typedef struct {
    struct PathDef entries[1];
    u16 pad;
} PathDefs1;

/* The ROM image carries a zero halfword of alignment padding after these
 * records, so it is written as part of the object. */
typedef struct {
    u16 value;
    u16 pad;
} ScalarEntryU16;

extern ScalarEntryU16 data_ov055_02111920;
extern ScalarEntryU16 data_ov055_02111924;
extern PathDefs1 data_ov055_02111928;
extern ObjTable data_ov055_02111930;
extern ObjTable data_ov055_02111938;
extern Area data_ov055_02111940[1];
extern PathNode data_ov055_0211194c[2];
extern ObjSubTable data_ov055_02111958[2];
extern StandardEntry data_ov055_02111968[1];
extern ViewEntry data_ov055_02111978[2];
extern LevelRec data_ov055_02111994;
extern Clps_021119b0 data_ov055_021119b0;
extern StandardEntry data_ov055_021119d0[3];
extern ObjSubTable data_ov055_02111a00[6];
extern SimpleEntry data_ov055_02111a30[12];

ScalarEntryU16 data_ov055_02111920 = {0x071f, 0x0000};

ScalarEntryU16 data_ov055_02111924 = {0x0a28, 0x0000};

ObjTable data_ov055_02111930 = {0x0002, {0x00, 0x00}, &data_ov055_02111958[0]};

PathDefs1 data_ov055_02111928 = {
    {{0x0000, 0x02, 0xff, 0x00, 0x00}},
    0x0000,
};

ObjTable data_ov055_02111938 = {0x0006, {0x00, 0x00}, &data_ov055_02111a00[0]};

Area data_ov055_02111940[1] = {
    {&data_ov055_02111930, 0, 0x00, {0x03, 0x08, 0x07}},
};

PathNode data_ov055_0211194c[2] = {
    {(s16)0xf704, 0x012c, (s16)0xfc81},
    {(s16)0xf704, 0x012c, 0x037f},
};

StandardEntry data_ov055_02111968[1] = {
    {0x0000, (s16)0xfa24, 0x06a4, 0x0000, 0x0000, 0x4000, 0x0000, 0x0300},
};

ObjSubTable data_ov055_02111958[2] = {
    {0x00, 0x03, {0x00, 0x00}, &data_ov055_021119d0[0]},
    {0x05, 0x0c, {0x00, 0x00}, &data_ov055_02111a30[0]},
};

ViewEntry data_ov055_02111978[2] = {
    {0x02, 0xff, (s16)0xfb50, 0x060e, 0x0064, {0x00, 0x00}, (s16)0xaaaa, 0x0000},
    {0x06, 0x00, (s16)0xf704, 0x0140, (s16)0xfd44, {0x00, 0x00}, 0x4000, 0x0000},
};

LevelRec data_ov055_02111994 = {(struct CLPS_Block *)&data_ov055_021119b0,
                                &data_ov055_02111938,
                                0x0720,
                                0x071e,
                                0x0721,
                                0x0722,
                                &data_ov055_02111940[0],
                                0x01,
                                0x00,
                                0x0226,
                                0x0000000f};

Clps_021119b0 data_ov055_021119b0 = {
    0x53504c43,
    0x0008,
    0x0003,
    {{0x00030fc4, 0x000000ff}, {0x00038044, 0x000000ff}, {0x00000fc0, 0x000000ff}}};

StandardEntry data_ov055_021119d0[3] = {
    {0x0044, 0x0190, 0x0000, 0x0384, 0x0000, (s16)0xc000, 0x0000, 0xff03},
    {0x0142, 0x0254, 0x0000, 0x01a6, 0x0000, (s16)0xc000, 0x0000, 0xffff},
    {0x00f4, (s16)0xf8f8, 0x0190, 0x0000, 0x0000, 0x4000, 0x0000, 0x0000},
};

ObjSubTable data_ov055_02111a00[6] = {
    {0x02, 0x02, {0x00, 0x00}, &data_ov055_0211194c[0]},
    {0x03, 0x01, {0x00, 0x00}, &data_ov055_02111928.entries[0]},
    {0x04, 0x02, {0x00, 0x00}, &data_ov055_02111978[0]},
    {0x0b, 0x01, {0x00, 0x00}, &data_ov055_02111920},
    {0x01, 0x01, {0x00, 0x00}, &data_ov055_02111968[0]},
    {0x0c, 0x01, {0x00, 0x00}, &data_ov055_02111924},
};

SimpleEntry data_ov055_02111a30[12] = {
    {0xe225, (s16)0xfaec, 0x0000, (s16)0xfaba}, {0xe225, (s16)0xfbb4, 0x0000, (s16)0xfaba},
    {0xe225, (s16)0xfaec, 0x0000, 0x0546},      {0xe225, (s16)0xfbb4, 0x0000, 0x0546},
    {0xe225, (s16)0xfc7c, 0x0000, (s16)0xfaba}, {0xe225, (s16)0xfd44, 0x0000, (s16)0xfaba},
    {0xe225, (s16)0xfe0c, 0x0000, (s16)0xfaba}, {0xe225, (s16)0xfc7c, 0x0000, 0x0546},
    {0xe225, (s16)0xfd44, 0x0000, 0x0546},      {0xe225, (s16)0xfe0c, 0x0000, 0x0546},
    {0xe225, (s16)0xfed4, 0x0000, (s16)0xfaba}, {0xe225, (s16)0xfed4, 0x0000, 0x0546},
};
