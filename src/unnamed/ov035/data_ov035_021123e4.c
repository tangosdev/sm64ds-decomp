#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov035_021123e4_Layout;
typedef char data_ov035_021123e4_size[sizeof(data_ov035_021123e4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[2];
} data_ov035_021123ec_Layout;
typedef char data_ov035_021123ec_size[sizeof(data_ov035_021123ec_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov035_021123f4_Layout;
typedef char data_ov035_021123f4_size[sizeof(data_ov035_021123f4_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov035_02112404_Layout;
typedef char data_ov035_02112404_size[sizeof(data_ov035_02112404_Layout) == 0x18 ? 1 : -1];

extern data_ov035_021123e4_Layout data_ov035_021123e4;
extern data_ov035_021123ec_Layout data_ov035_021123ec;
extern data_ov035_021123f4_Layout data_ov035_021123f4;
extern data_ov035_02112404_Layout data_ov035_02112404;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_021123e4_Layout data_ov035_021123e4 = {{
    0xffffffff,
    0xffffffff,
}};

data_ov035_021123ec_Layout data_ov035_021123ec = {{
    0xf8eec43f,
    0xfc9a0082,
}};

data_ov035_021123f4_Layout data_ov035_021123f4 = {{
    0xfbdc0a3d,
    0xfca908b6,
    0x092e083d,
    0x092e1be9,
}};

data_ov035_02112404_Layout data_ov035_02112404 = {{
    0xfa4a023d,
    0xfb5ffb3c,
    0xfd30e225,
    0xfd30f128,
    0xfd30e225,
    0xfd30f1be,
}};
