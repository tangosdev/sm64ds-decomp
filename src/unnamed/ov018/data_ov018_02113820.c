#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov018_02113820_Layout;
typedef char data_ov018_02113820_size[sizeof(data_ov018_02113820_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[23];
} data_ov018_0211382c_Layout;
typedef char data_ov018_0211382c_size[sizeof(data_ov018_0211382c_Layout) == 0x5c ? 1 : -1];

extern data_ov018_02113820_Layout data_ov018_02113820;
extern data_ov018_0211382c_Layout data_ov018_0211382c;

data_ov018_02113820_Layout data_ov018_02113820 = {{
    0x00000000,
    0xffff0000,
    0xfc2c002c,
}};

data_ov018_0211382c_Layout data_ov018_0211382c = {{
    0x16fdf254, 0x00000000, 0xff070000, 0xf2df0017, 0x0500fa83, 0x00000000, 0xffff0000, 0x04c200ef,
    0x14e1f0c4, 0x00000000, 0xffff0000, 0xf1e90017, 0x05dafa7e, 0x00000000, 0xffff0000, 0xf1cd0017,
    0x01a1faae, 0x00000000, 0xffff0000, 0x0b9200bc, 0x006fee18, 0x35550000, 0xffff0000,
}};
