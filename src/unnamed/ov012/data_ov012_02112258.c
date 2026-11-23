#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} data_ov012_02112258_Layout;
typedef char data_ov012_02112258_size[sizeof(data_ov012_02112258_Layout) == 0x24 ? 1 : -1];

extern data_ov012_02112258_Layout data_ov012_02112258;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02112258_Layout data_ov012_02112258 = {{
    0xfaecf0c4,
    0xf18cfda8,
    0xfc7cfac2,
    0xfb1ef254,
    0xf8f802bc,
    0x0190fb82,
    0xfb82f8f8,
    0xfa88fa88,
    0xf95cfb82,
}};
