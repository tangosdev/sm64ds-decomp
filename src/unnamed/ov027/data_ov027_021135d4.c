#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov027_021135d4_Layout;
typedef char data_ov027_021135d4_size[sizeof(data_ov027_021135d4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[56];
} data_ov027_021135f4_Layout;
typedef char data_ov027_021135f4_size[sizeof(data_ov027_021135f4_Layout) == 0xe0 ? 1 : -1];

extern data_ov027_021135d4_Layout data_ov027_021135d4;
extern data_ov027_021135f4_Layout data_ov027_021135f4;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021135d4_Layout data_ov027_021135d4 = {{
    0xe7a70039,
    0xf3e407d0,
    0x00000000,
    0x01ff0000,
    0xf6a00095,
    0xf0600476,
    0x00000000,
    0xffff0000,
}};

data_ov027_021135f4_Layout data_ov027_021135f4 = {{
    0xeffc0103, 0x0b5403b5, 0x00000000, 0xffff0000, 0xeffc0103, 0x0ed803b5, 0x00000000, 0xffff0000,
    0x00000031, 0x000014e6, 0x00000000, 0x00170000, 0x00000031, 0x000014e6, 0x00000000, 0x00f80000,
    0x1117001d, 0x04710320, 0x00000000, 0xff010000, 0x041a00a5, 0x041a0e42, 0x00000000, 0xffff0000,
    0x00c8001e, 0x00c81388, 0x20000000, 0xff050000, 0x0ba900fe, 0x14b903e8, 0x80000000, 0x00040000,
    0xf94d00fe, 0xf060047c, 0x40000000, 0x02040000, 0x11a300fe, 0xfbf00454, 0x20000000, 0x01040000,
    0x1023000f, 0xea9303ee, 0x80000000, 0xf1f00000, 0x0f4c000f, 0xe88503e8, 0x00000000, 0xf1f10000,
    0x1107000f, 0xe89103e8, 0x00000000, 0xf1f20000, 0x10040108, 0xe95b041a, 0x00000000, 0xffff0000,
}};
