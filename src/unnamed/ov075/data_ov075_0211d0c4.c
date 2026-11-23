#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov075_0211d0c4_Layout;
typedef char data_ov075_0211d0c4_size[sizeof(data_ov075_0211d0c4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[15];
} data_ov075_0211d0c8_Layout;
typedef char data_ov075_0211d0c8_size[sizeof(data_ov075_0211d0c8_Layout) == 0x3c ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov075_0211d104_Layout;
typedef char data_ov075_0211d104_size[sizeof(data_ov075_0211d104_Layout) == 0x40 ? 1 : -1];

extern data_ov075_0211d0c4_Layout data_ov075_0211d0c4;
extern data_ov075_0211d0c8_Layout data_ov075_0211d0c8;
extern data_ov075_0211d104_Layout data_ov075_0211d104;

#pragma define_section sName ".data.entry_sprite_gap10"                                            \
                             ".data.entry_sprite_gap10"                                            \
                             ".data.entry_sprite_gap10" far_data
#pragma section sName begin

data_ov075_0211d0c4_Layout data_ov075_0211d0c4 = {{
    0x818d00d0,
}};

data_ov075_0211d0c8_Layout data_ov075_0211d0c8 = {{
    0x00000300,
    0x81ad00d0,
    0x00000304,
    0x81cd00d0,
    0x00000308,
    0x81e700d0,
    0x0000030c,
    0x800200d0,
    0x00000310,
    0x802100d0,
    0x00000310,
    0x803e00d0,
    0x00000314,
    0x805800d0,
    0xffff0318,
}};

data_ov075_0211d104_Layout data_ov075_0211d104 = {{
    0x818900d0,
    0x00001380,
    0x81a800d0,
    0x00001304,
    0x81c700d0,
    0x00001384,
    0x81e600d0,
    0x00001388,
    0x81ff00d0,
    0x0000130c,
    0x801900d0,
    0x00001304,
    0x803800d0,
    0x00001384,
    0x805600d0,
    0xffff1314,
}};
