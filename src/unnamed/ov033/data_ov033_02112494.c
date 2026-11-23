#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov033_02112494_Layout;
typedef char data_ov033_02112494_size[sizeof(data_ov033_02112494_Layout) == 0x18 ? 1 : -1];

extern data_ov033_02112494_Layout data_ov033_02112494;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112494_Layout data_ov033_02112494 = {{
    0x02010148,
    0x02010144,
    0x02010140,
    0x0201013c,
    0x02010138,
    0x02010134,
}};
