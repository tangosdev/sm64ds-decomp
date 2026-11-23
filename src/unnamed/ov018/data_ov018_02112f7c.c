#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov018_02112f7c_Layout;
typedef char data_ov018_02112f7c_size[sizeof(data_ov018_02112f7c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov018_02112f88_Layout;
typedef char data_ov018_02112f88_size[sizeof(data_ov018_02112f88_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov018_02112fa8_Layout;
typedef char data_ov018_02112fa8_size[sizeof(data_ov018_02112fa8_Layout) == 0x20 ? 1 : -1];

extern data_ov018_02112f7c_Layout data_ov018_02112f7c;
extern data_ov018_02112f88_Layout data_ov018_02112f88;
extern data_ov018_02112fa8_Layout data_ov018_02112fa8;

data_ov018_02112f7c_Layout data_ov018_02112f7c = {{
    0x01ecee6c,
    0x40000000,
    0xf1f00000,
}};

data_ov018_02112f88_Layout data_ov018_02112f88 = {{
    0xeffc00a2,
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
    0x1460000f,
    0x01ecee6c,
    0x40000000,
    0xf1f00000,
}};

data_ov018_02112fa8_Layout data_ov018_02112fa8 = {{
    0xeffc00a2,
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
    0x1460000f,
    0x01ecee6c,
    0x40000000,
    0xf1f00000,
}};
