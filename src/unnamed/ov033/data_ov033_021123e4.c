#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov033_021123e4_Layout;
typedef char data_ov033_021123e4_size[sizeof(data_ov033_021123e4_Layout) == 0x10 ? 1 : -1];

extern data_ov033_021123e4_Layout data_ov033_021123e4;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_021123e4_Layout data_ov033_021123e4 = {{
    0x02010134,
    0x02010130,
    0x0201012c,
    0x02010124,
}};
