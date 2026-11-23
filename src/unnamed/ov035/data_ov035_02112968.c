#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[19];
} data_ov035_02112968_Layout;
typedef char data_ov035_02112968_size[sizeof(data_ov035_02112968_Layout) == 0x4c ? 1 : -1];

typedef struct {
    u32 w[46];
} data_ov035_021129b4_Layout;
typedef char data_ov035_021129b4_size[sizeof(data_ov035_021129b4_Layout) == 0xb8 ? 1 : -1];

extern data_ov035_02112968_Layout data_ov035_02112968;
extern data_ov035_021129b4_Layout data_ov035_021129b4;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112968_Layout data_ov035_02112968 = {{
    0xff38f6e6, 0x00000000, 0xffff0000, 0x06a40131, 0xfed4ffec, 0x00000000, 0xff010000,
    0xfd30013b, 0x024bed9a, 0x00010002, 0x0020ffff, 0xfd30013c, 0x024bf056, 0x00000000,
    0xffc003e0, 0xfbb4000f, 0xfd44040f, 0x00000000, 0xf1f10000,
}};

data_ov035_021129b4_Layout data_ov035_021129b4 = {{
    0xfbc900fe, 0x06280bb8, 0x78e30000, 0x12040000, 0x04b0000f, 0x02bcffec, 0x00000000, 0xffff0000,
    0x0578000f, 0x00c8ffec, 0x00000000, 0xffff0000, 0x00c8000f, 0xf95cffec, 0x00000000, 0xf1f00000,
    0xfa24000f, 0xfe0cffec, 0x00000000, 0xffff0000, 0xface011c, 0xfa9c0810, 0x200000b6, 0xffff0000,
    0x01f4002c, 0x07080000, 0xa0000000, 0x1e020000, 0xfbfa001e, 0xfc720509, 0x20000000, 0xff060000,
    0x0640004c, 0xff38ed9a, 0xe000ffff, 0x07550000, 0x05c80059, 0x05c8170c, 0x20000000, 0xffff0000,
    0x00c800fe, 0xf95cffec, 0x00000000, 0x0f000000, 0xfbb400fe, 0xfd44040f,
}};
