#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov018_02112fc8_Layout;
typedef char data_ov018_02112fc8_size[sizeof(data_ov018_02112fc8_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[10];
} data_ov018_02112fd0_Layout;
typedef char data_ov018_02112fd0_size[sizeof(data_ov018_02112fd0_Layout) == 0x28 ? 1 : -1];

extern data_ov018_02112fc8_Layout data_ov018_02112fc8;
extern data_ov018_02112fd0_Layout data_ov018_02112fd0;

data_ov018_02112fc8_Layout data_ov018_02112fc8 = {{
    0x1460000f,
    0x01ecee6c,
}};

data_ov018_02112fd0_Layout data_ov018_02112fd0 = {{
    0x40000000,
    0xf1f00000,
    0x0a280134,
    0x10ccee6c,
    0x00000000,
    0xff010000,
    0xeffc00a2,
    0x06fefb3c,
    0xc0000000,
    0xffff0000,
}};
