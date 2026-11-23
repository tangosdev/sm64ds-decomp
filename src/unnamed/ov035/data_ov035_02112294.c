#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} data_ov035_02112294_Layout;
typedef char data_ov035_02112294_size[sizeof(data_ov035_02112294_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
} data_ov035_0211229c_Layout;
typedef char data_ov035_0211229c_size[sizeof(data_ov035_0211229c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[12];
} data_ov035_021122a8_Layout;
typedef char data_ov035_021122a8_size[sizeof(data_ov035_021122a8_Layout) == 0x30 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov035_021122d8_Layout;
typedef char data_ov035_021122d8_size[sizeof(data_ov035_021122d8_Layout) == 0x40 ? 1 : -1];

extern data_ov035_02112294_Layout data_ov035_02112294;
extern data_ov035_0211229c_Layout data_ov035_0211229c;
extern data_ov035_021122a8_Layout data_ov035_021122a8;
extern data_ov035_021122d8_Layout data_ov035_021122d8;
extern u8 data_ov035_0211205c[];
extern u8 data_ov035_02112040[];
extern u8 data_ov035_02112048[];

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112294_Layout data_ov035_02112294 = {
    &data_ov035_0211205c[0],
    0x0000010c,
};

data_ov035_0211229c_Layout data_ov035_0211229c = {
    &data_ov035_02112040[0],
    0x0000010e,
    &data_ov035_02112048[0],
};

data_ov035_021122a8_Layout data_ov035_021122a8 = {{
    0x044c2e3e,
    0x02bc20d0,
    0x00002e3e,
    0x00000fa0,
    0x05782e3e,
    0x00001770,
    0xfe0c2e3e,
    0xfaec1d4c,
    0xfaec2e3e,
    0xfbb41388,
    0x0282ce3d,
    0x072805aa,
}};

data_ov035_021122d8_Layout data_ov035_021122d8 = {{
    0x05be0000,
    0x0080f43e,
    0x9c720000,
    0x05280000,
    0x06ea0000,
    0x0080f43e,
    0x9c720000,
    0x05280000,
    0x05be0000,
    0x01acf43e,
    0x9c720000,
    0x05280000,
    0x06ea0000,
    0x01acf43e,
    0x9c720000,
    0x05280000,
}};
