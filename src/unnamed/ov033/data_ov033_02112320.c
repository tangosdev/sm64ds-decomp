#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov033_02112320_Layout;
typedef char data_ov033_02112320_size[sizeof(data_ov033_02112320_Layout) == 0x1c ? 1 : -1];

extern data_ov033_02112320_Layout data_ov033_02112320;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112320_Layout data_ov033_02112320 = {{
    0xf8bffe3e,
    0x00000000,
    0x02040000,
    0xfaec00fe,
    0xfd7600d2,
    0x00000000,
    0x01040000,
}};
