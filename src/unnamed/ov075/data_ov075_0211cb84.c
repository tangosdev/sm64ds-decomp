#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov075_0211cb84_Layout;
typedef char data_ov075_0211cb84_size[sizeof(data_ov075_0211cb84_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov075_0211cba4_Layout;
typedef char data_ov075_0211cba4_size[sizeof(data_ov075_0211cba4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov075_0211cbc4_Layout;
typedef char data_ov075_0211cbc4_size[sizeof(data_ov075_0211cbc4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov075_0211cbe4_Layout;
typedef char data_ov075_0211cbe4_size[sizeof(data_ov075_0211cbe4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov075_0211cc04_Layout;
typedef char data_ov075_0211cc04_size[sizeof(data_ov075_0211cc04_Layout) == 0x20 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
    void *f018;
    u32 f01c;
} data_ov075_0211cc24_Layout;
typedef char data_ov075_0211cc24_size[sizeof(data_ov075_0211cc24_Layout) == 0x20 ? 1 : -1];

extern data_ov075_0211cb84_Layout data_ov075_0211cb84;
extern data_ov075_0211cba4_Layout data_ov075_0211cba4;
extern data_ov075_0211cbc4_Layout data_ov075_0211cbc4;
extern data_ov075_0211cbe4_Layout data_ov075_0211cbe4;
extern data_ov075_0211cc04_Layout data_ov075_0211cc04;
extern data_ov075_0211cc24_Layout data_ov075_0211cc24;
extern u8 data_ov075_0211cef4[];
extern u8 data_ov075_0211cf24[];
extern u8 data_ov075_0211cf54[];
extern u8 data_ov075_0211cf84[];

#pragma define_section sName ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9" far_data
#pragma section sName begin

data_ov075_0211cc04_Layout data_ov075_0211cc04 = {{
    0x11de00f4,
    0x0000e11b,
    0x11d50001,
    0x0000e11c,
    0x11c5000e,
    0x0000e11e,
    0x11cd00f9,
    0xffffe11d,
}};

data_ov075_0211cbe4_Layout data_ov075_0211cbe4 = {{
    0x000a00f4,
    0x0000e11b,
    0x00130001,
    0x0000e11c,
    0x0023000e,
    0x0000e11e,
    0x001b00f9,
    0xffffe11d,
}};

data_ov075_0211cbc4_Layout data_ov075_0211cbc4 = {{
    0x81f000d8,
    0x0000a143,
    0x801080d8,
    0x0000a147,
    0xc1d800e8,
    0x0000a018,
    0xc00080e8,
    0xffffa18b,
}};

data_ov075_0211cba4_Layout data_ov075_0211cba4 = {{
    0xc18040f0,
    0x00000000,
    0xc1c040f0,
    0x00000008,
    0xc00040f0,
    0x00000010,
    0xc04040f0,
    0xffff0018,
}};

data_ov075_0211cb84_Layout data_ov075_0211cb84 = {{
    0xc18040f0,
    0x00002000,
    0xc1c040f0,
    0x00002008,
    0xc00040f0,
    0x00002010,
    0xc04040f0,
    0xffff2018,
}};

data_ov075_0211cc24_Layout data_ov075_0211cc24 = {
    &data_ov075_0211cef4[0], 0x00000004, &data_ov075_0211cf24[0], 0x00000002,
    &data_ov075_0211cf54[0], 0x00000004, &data_ov075_0211cf84[0], 0x00000002,
};
