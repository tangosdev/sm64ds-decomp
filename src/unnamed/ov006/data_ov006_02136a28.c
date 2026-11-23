#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov006_02136a28_Layout;
typedef char data_ov006_02136a28_size[sizeof(data_ov006_02136a28_Layout) == 0x8 ? 1 : -1];

extern data_ov006_02136a28_Layout data_ov006_02136a28;

#pragma define_section sName ".data.gap21"                                                         \
                             ".data.gap21"                                                         \
                             ".data.gap21" far_data
#pragma section sName begin

data_ov006_02136a28_Layout data_ov006_02136a28 = {{
    0x91f000f0,
    0xffff0008,
}};
