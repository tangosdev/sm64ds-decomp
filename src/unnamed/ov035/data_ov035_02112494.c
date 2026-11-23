#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov035_02112494_Layout;
typedef char data_ov035_02112494_size[sizeof(data_ov035_02112494_Layout) == 0x18 ? 1 : -1];

typedef struct {
    u32 w[6];
} data_ov035_021124ac_Layout;
typedef char data_ov035_021124ac_size[sizeof(data_ov035_021124ac_Layout) == 0x18 ? 1 : -1];

extern data_ov035_02112494_Layout data_ov035_02112494;
extern data_ov035_021124ac_Layout data_ov035_021124ac;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112494_Layout data_ov035_02112494 = {{
    0xff6a2227,
    0xf9f2ffec,
    0x00962227,
    0xf9f2ffec,
    0xfe3e2227,
    0xf8f8ffec,
}};

data_ov035_021124ac_Layout data_ov035_021124ac = {{
    0xff6a2227,
    0xf8f8ffec,
    0x00962227,
    0xf8f8ffec,
    0x01c22227,
    0xf8f8ffec,
}};
