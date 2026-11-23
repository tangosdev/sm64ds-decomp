#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[18];
} data_ov075_0211d264_Layout;
typedef char data_ov075_0211d264_size[sizeof(data_ov075_0211d264_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[20];
} data_ov075_0211d2ac_Layout;
typedef char data_ov075_0211d2ac_size[sizeof(data_ov075_0211d2ac_Layout) == 0x50 ? 1 : -1];

extern data_ov075_0211d264_Layout data_ov075_0211d264;
extern data_ov075_0211d2ac_Layout data_ov075_0211d2ac;

#pragma define_section sName ".data.entry_sprite_gap11"                                            \
                             ".data.entry_sprite_gap11"                                            \
                             ".data.entry_sprite_gap11" far_data
#pragma section sName begin

data_ov075_0211d264_Layout data_ov075_0211d264 = {{
    0x819c00d0,
    0x00001302,
    0x81b100d0,
    0x00001306,
    0x81cf00d0,
    0x00001380,
    0x81e400d0,
    0x00001388,
    0x81f900d0,
    0x00001384,
    0x800e00d0,
    0x00001312,
    0x802680d0,
    0x0000138c,
    0x802f00d0,
    0x00001312,
    0x804400d0,
    0xffff1316,
}};

data_ov075_0211d2ac_Layout data_ov075_0211d2ac = {{
    0x819980d0, 0x00000300, 0x81a300d0, 0x00000302, 0x81b800d0, 0x00000306, 0x81d500d0,
    0x0000030a, 0x81ea00d0, 0x00000306, 0x81ff00d0, 0x0000030e, 0x801400d0, 0x00000306,
    0x802900d0, 0x00000312, 0x803e00d0, 0x00000316, 0x805780d0, 0xffff031a,
}};
