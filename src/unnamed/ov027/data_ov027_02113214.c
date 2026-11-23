#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} data_ov027_02113214_Layout;
typedef char data_ov027_02113214_size[sizeof(data_ov027_02113214_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov027_02113228_Layout;
typedef char data_ov027_02113228_size[sizeof(data_ov027_02113228_Layout) == 0x3c ? 1 : -1];

typedef struct {
    u32 w[34];
} data_ov027_02113264_Layout;
typedef char data_ov027_02113264_size[sizeof(data_ov027_02113264_Layout) == 0x88 ? 1 : -1];

extern data_ov027_02113214_Layout data_ov027_02113214;
extern data_ov027_02113228_Layout data_ov027_02113228;
extern data_ov027_02113264_Layout data_ov027_02113264;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_02113214_Layout data_ov027_02113214 = {{
    0x15180000,
    0x02580a8c,
    0xc0000000,
    0x05000000,
    0x15180000,
}};

data_ov027_02113228_Layout data_ov027_02113228 = {{
    0x006403e8,
    0xc0000000,
    0x00000000,
    0x15180000,
    0xfe7003e8,
    0xc0000000,
    0x00000000,
    0x15180000,
    0xfc7c03e8,
    0xc0000000,
    0x00000000,
    0x00c80000,
    0x05780834,
    0xcaab0000,
    0x00080000,
}};

data_ov027_02113264_Layout data_ov027_02113264 = {{
    0x53504c43, 0x00180008, 0x01000fc0, 0x000000ff, 0x000c0fc7, 0x000000ff, 0x00040fe0,
    0x000000ff, 0x00040fc4, 0x000000ff, 0x01044fc6, 0x000000ff, 0x01044fc0, 0x000000ff,
    0x01040fc1, 0x000000ff, 0x00052fc5, 0x000000ff, 0x00053fc7, 0x000000ff, 0x005c0fc6,
    0x00000000, 0x01744fc7, 0x000000ff, 0x01744fc5, 0x000000ff, 0x00744fc7, 0x000000ff,
    0x00744fc1, 0x000000ff, 0x00744fc4, 0x000000ff, 0x00742fc8, 0x000000ff,
}};
