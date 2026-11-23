#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov035_021124e4_Layout;
typedef char data_ov035_021124e4_size[sizeof(data_ov035_021124e4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov035_021124ec_Layout;
typedef char data_ov035_021124ec_size[sizeof(data_ov035_021124ec_Layout) == 0xc ? 1 : -1];

extern data_ov035_021124e4_Layout data_ov035_021124e4;
extern data_ov035_021124ec_Layout data_ov035_021124ec;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_021124e4_Layout data_ov035_021124e4 = {{
    0xf8dd0056,
    0x011007e4,
}};

data_ov035_021124ec_Layout data_ov035_021124ec = {{
    0x40000000,
    0xffff0000,
    0xfd260056,
}};
