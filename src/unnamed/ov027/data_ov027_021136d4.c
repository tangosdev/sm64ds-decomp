#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov027_021136d4_Layout;
typedef char data_ov027_021136d4_size[sizeof(data_ov027_021136d4_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[18];
} data_ov027_021136f4_Layout;
typedef char data_ov027_021136f4_size[sizeof(data_ov027_021136f4_Layout) == 0x48 ? 1 : -1];

typedef struct {
    u32 w[34];
} data_ov027_0211373c_Layout;
typedef char data_ov027_0211373c_size[sizeof(data_ov027_0211373c_Layout) == 0x88 ? 1 : -1];

extern data_ov027_021136d4_Layout data_ov027_021136d4;
extern data_ov027_021136f4_Layout data_ov027_021136f4;
extern data_ov027_0211373c_Layout data_ov027_0211373c;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021136d4_Layout data_ov027_021136d4 = {{
    0x0e74011d,
    0x100404b0,
    0x00000000,
    0xffff0000,
    0x10cc011d,
    0x100405dc,
    0x00000000,
    0xffff0000,
}};

data_ov027_021136f4_Layout data_ov027_021136f4 = {{
    0x0967001e,
    0xf84606d6,
    0x00000000,
    0xff070000,
    0x0a3c004c,
    0xf96e05ba,
    0x0000ffff,
    0x07520000,
    0x10cc011d,
    0x100404b0,
    0x00000000,
    0xffff0000,
    0x0e74011d,
    0x125c0708,
    0x00000000,
    0xffff0000,
    0x0e74011d,
    0x125c05dc,
}};

data_ov027_0211373c_Layout data_ov027_0211373c = {{
    0x00000000, 0xffff0000, 0x0e74011d, 0x125c04b0, 0x00000000, 0xffff0000, 0x10cc011d,
    0x125c0708, 0x00000000, 0xffff0000, 0x10cc011d, 0x125c05dc, 0x00000000, 0xffff0000,
    0x10cc011d, 0x125c04b0, 0x00000000, 0xffff0000, 0x126d001c, 0x0d6803e8, 0x00000000,
    0xffff0000, 0xec19001c, 0xe6ce03e8, 0x00000000, 0xffff0000, 0xee3900bb, 0xf5d203fc,
    0x40000000, 0xffff0000, 0x0bab00bc, 0x00eb03e8, 0x40000000, 0xffff0000,
}};
