#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov025_021137e4_Layout;
typedef char data_ov025_021137e4_size[sizeof(data_ov025_021137e4_Layout) == 0x8 ? 1 : -1];

extern data_ov025_021137e4_Layout data_ov025_021137e4;

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_021137e4_Layout data_ov025_021137e4 = {{
    0x635f6b6b,
    0x00000000,
}};
