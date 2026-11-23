#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[5];
} data_ov033_021124ac_Layout;
typedef char data_ov033_021124ac_size[sizeof(data_ov033_021124ac_Layout) == 0x14 ? 1 : -1];

extern data_ov033_021124ac_Layout data_ov033_021124ac;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_021124ac_Layout data_ov033_021124ac = {{
    0x02010130,
    0x0201012c,
    0x02010124,
    0x020100dc,
    0x020ee55c,
}};
