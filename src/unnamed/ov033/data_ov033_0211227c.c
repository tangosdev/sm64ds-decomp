#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov033_0211227c_Layout;
typedef char data_ov033_0211227c_size[sizeof(data_ov033_0211227c_Layout) == 0x18 ? 1 : -1];

extern data_ov033_0211227c_Layout data_ov033_0211227c;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_0211227c_Layout data_ov033_0211227c = {{
    0x08340010,
    0xfbb4fe3e,
    0x00000000,
    0xffff0000,
    0x08340010,
    0x012cfd12,
}};
