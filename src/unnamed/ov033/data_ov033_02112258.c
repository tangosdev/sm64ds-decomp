#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov033_02112258_Layout;
typedef char data_ov033_02112258_size[sizeof(data_ov033_02112258_Layout) == 0x24 ? 1 : -1];

extern data_ov033_02112258_Layout data_ov033_02112258;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112258_Layout data_ov033_02112258 = {{
    0xffff0000,
    0xfe0c0010,
    0xff9c0285,
    0x00000000,
    0xffff0000,
    0x08340010,
    0xfa88fe3e,
    0x00000000,
    0xffff0000,
}};
