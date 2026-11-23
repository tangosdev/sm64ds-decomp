#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov035_02112a6c_Layout;
typedef char data_ov035_02112a6c_size[sizeof(data_ov035_02112a6c_Layout) == 0x18 ? 1 : -1];

extern data_ov035_02112a6c_Layout data_ov035_02112a6c;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112a6c_Layout data_ov035_02112a6c = {{
    0x00000000,
    0x0f010000,
    0xfbc900fe,
    0x06280bb8,
    0x78e30000,
    0x02040000,
}};
