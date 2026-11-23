#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} data_ov035_02112258_Layout;
typedef char data_ov035_02112258_size[sizeof(data_ov035_02112258_Layout) == 0x20 ? 1 : -1];

extern data_ov035_02112258_Layout data_ov035_02112258;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112258_Layout data_ov035_02112258 = {{
    0x53504c43,
    0x00010008,
    0x00000fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
