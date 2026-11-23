#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov075_0211cfe4_Layout;
typedef char data_ov075_0211cfe4_size[sizeof(data_ov075_0211cfe4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
    void *f014;
    u32 f018;
    void *f01c;
    u32 f020;
    u32 f024;
    u32 f028;
} data_ov075_0211cfe8_Layout;
typedef char data_ov075_0211cfe8_size[sizeof(data_ov075_0211cfe8_Layout) == 0x2c ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov075_0211d014_Layout;
typedef char data_ov075_0211d014_size[sizeof(data_ov075_0211d014_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov075_0211d04c_Layout;
typedef char data_ov075_0211d04c_size[sizeof(data_ov075_0211d04c_Layout) == 0x38 ? 1 : -1];

extern data_ov075_0211cfe4_Layout data_ov075_0211cfe4;
extern data_ov075_0211cfe8_Layout data_ov075_0211cfe8;
extern data_ov075_0211d014_Layout data_ov075_0211d014;
extern data_ov075_0211d04c_Layout data_ov075_0211d04c;
extern u8 data_ov075_0211c7f8[];
extern u8 data_ov075_0211ca94[];
extern u8 data_ov075_0211cc04[];
extern u8 data_ov075_0211c934[];
extern u8 data_ov075_0211c7e0[];

#pragma define_section sName ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9"                                             \
                             ".data.entry_sprite_gap9" far_data
#pragma section sName begin

data_ov075_0211cfe4_Layout data_ov075_0211cfe4 = {
    &data_ov075_0211c7f8[0],
};

data_ov075_0211cfe8_Layout data_ov075_0211cfe8 = {
    0x00000006, &data_ov075_0211ca94[0],
    0x00000006, &data_ov075_0211cc04[0],
    0x00000005, &data_ov075_0211c934[0],
    0x00000004, &data_ov075_0211c7e0[0],
    0x00000003, 0x00000000,
    0x0000000a,
};

data_ov075_0211d014_Layout data_ov075_0211d014 = {{
    0x819900d0,
    0x00000300,
    0x81c000d0,
    0x00000304,
    0x81de00d0,
    0x00000300,
    0x81fc00d0,
    0x00000304,
    0x801d00d0,
    0x0000030c,
    0x803b00d0,
    0x00000310,
    0x805d80d0,
    0xffff0314,
}};

data_ov075_0211d04c_Layout data_ov075_0211d04c = {{
    0x819300d0,
    0x00001300,
    0x81bd00d0,
    0x00001380,
    0x81dc00d0,
    0x00001310,
    0x81fb00d0,
    0x00001384,
    0x801a00d0,
    0x00001388,
    0x803800d0,
    0x00001308,
    0x805c80d0,
    0xffff1314,
}};
