#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov012_02112294_Layout;
typedef char data_ov012_02112294_size[sizeof(data_ov012_02112294_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 w[25];
} data_ov012_0211229c_Layout;
typedef char data_ov012_0211229c_size[sizeof(data_ov012_0211229c_Layout) == 0x64 ? 1 : -1];

extern data_ov012_02112294_Layout data_ov012_02112294;
extern data_ov012_0211229c_Layout data_ov012_0211229c;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02112294_Layout data_ov012_02112294 = {{
    0xfb5cf448,
    0xf510f830,
}};

data_ov012_0211229c_Layout data_ov012_0211229c = {{
    0xf380fb50, 0xfb50fa88, 0xfe70f448, 0xf380fb50, 0xfb5000c8, 0xfed4f5d8, 0xf768fb50,
    0xfb1ef0c4, 0xf1f002bc, 0xfaecfaba, 0xfb1ef1f0, 0xf5d801f4, 0x02bcfb34, 0xfb1ef448,
    0xf2b804b0, 0x02bcfb1e, 0xf66e0d48, 0x0af0fd44, 0xfe0cf66e, 0xf66e0898, 0x0898fce0,
    0xfed4f66e, 0xf66e0b54, 0x0d48ff38, 0xfed4f66e,
}};
