#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov052_02112294_Layout;
typedef char data_ov052_02112294_size[sizeof(data_ov052_02112294_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[33];
} data_ov052_0211229c_Layout;
typedef char data_ov052_0211229c_size[sizeof(data_ov052_0211229c_Layout) == 0x84 ? 1 : -1];

extern data_ov052_02112294_Layout data_ov052_02112294;
extern data_ov052_0211229c_Layout data_ov052_0211229c;

data_ov052_02112294_Layout data_ov052_02112294 = {{
    0xfa240017,
    0xfc7c0fa0,
}};

data_ov052_0211229c_Layout data_ov052_0211229c = {{
    0x00000000, 0xffff0000, 0xfaec0017, 0x012c0fa0, 0x00000000, 0xffff0000, 0xf510000f,
    0x041a0fa0, 0x00000000, 0xffff0000, 0x041a000f, 0xff9c0fa0, 0x00000000, 0xffff0000,
    0x1527003b, 0xf4ac12c0, 0x00000000, 0xffff0000, 0xf830000f, 0xfc7c0fa0, 0x00000000,
    0xffff0000, 0xfb82000f, 0xfed40fa0, 0x00000000, 0xffff0000, 0x073a000f, 0x0ce41004,
    0xce390000, 0xffff0000, 0x11ad004c, 0xfe251036, 0xf000ffff,
}};
