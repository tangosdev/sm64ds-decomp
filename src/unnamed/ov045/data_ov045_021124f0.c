#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov045_021124f0_Layout;
typedef char data_ov045_021124f0_size[sizeof(data_ov045_021124f0_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} data_ov045_021124fc_Layout;
typedef char data_ov045_021124fc_size[sizeof(data_ov045_021124fc_Layout) == 0x14 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov045_02112510_Layout;
typedef char data_ov045_02112510_size[sizeof(data_ov045_02112510_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov045_02112530_Layout;
typedef char data_ov045_02112530_size[sizeof(data_ov045_02112530_Layout) == 0x20 ? 1 : -1];

extern data_ov045_021124f0_Layout data_ov045_021124f0;
extern data_ov045_021124fc_Layout data_ov045_021124fc;
extern data_ov045_02112510_Layout data_ov045_02112510;
extern data_ov045_02112530_Layout data_ov045_02112530;

#pragma define_section sName ".data.w12460"                                                        \
                             ".data.w12460"                                                        \
                             ".data.w12460" far_data
#pragma section sName begin

data_ov045_021124f0_Layout data_ov045_021124f0 = {{
    0x53504c43,
    0x00020008,
    0x00002fc0,
}};

data_ov045_021124fc_Layout data_ov045_021124fc = {{
    0x000000ff,
    0x00000fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
}};

data_ov045_02112510_Layout data_ov045_02112510 = {{
    0x53504c43,
    0x00010008,
    0x01880fcc,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};

data_ov045_02112530_Layout data_ov045_02112530 = {{
    0x53504c43,
    0x00010008,
    0x00000fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
