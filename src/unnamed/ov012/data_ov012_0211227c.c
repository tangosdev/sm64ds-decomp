#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov012_0211227c_Layout;
typedef char data_ov012_0211227c_size[sizeof(data_ov012_0211227c_Layout) == 0x18 ? 1 : -1];

extern data_ov012_0211227c_Layout data_ov012_0211227c;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_0211227c_Layout data_ov012_0211227c = {{
    0xfb50fa88,
    0xf95cf510,
    0xf4acfb50,
    0xfb50f5d8,
    0xf510f510,
    0xfa24fb82,
}};
