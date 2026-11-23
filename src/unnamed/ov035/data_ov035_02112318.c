#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov035_02112318_Layout;
typedef char data_ov035_02112318_size[sizeof(data_ov035_02112318_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[14];
} data_ov035_02112320_Layout;
typedef char data_ov035_02112320_size[sizeof(data_ov035_02112320_Layout) == 0x38 ? 1 : -1];

extern data_ov035_02112318_Layout data_ov035_02112318;
extern data_ov035_02112320_Layout data_ov035_02112320;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112318_Layout data_ov035_02112318 = {{
    0x0000ff01,
    0x00001964,
}};

data_ov035_02112320_Layout data_ov035_02112320 = {{
    0x00000000,
    0xff020000,
    0x0797fb50,
    0x00000000,
    0x0000c000,
    0x04e2ff02,
    0x05dc0dac,
    0x80000000,
    0xff020000,
    0x10befce0,
    0x0000012c,
    0x00000000,
    0x05beff02,
    0x05be18c9,
}};
