#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[14];
} data_ov012_02111fc0_Layout;
typedef char data_ov012_02111fc0_size[sizeof(data_ov012_02111fc0_Layout) == 0x38 ? 1 : -1];

typedef struct {
    u32 w[37];
} data_ov012_02111ff8_Layout;
typedef char data_ov012_02111ff8_size[sizeof(data_ov012_02111ff8_Layout) == 0x94 ? 1 : -1];

extern data_ov012_02111fc0_Layout data_ov012_02111fc0;
extern data_ov012_02111ff8_Layout data_ov012_02111ff8;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02111fc0_Layout data_ov012_02111fc0 = {{
    0x40000000,
    0x042c0000,
    0x08fc0000,
    0x079efe0c,
    0x40000000,
    0x02090000,
    0x0fd20000,
    0x079efb82,
    0xc0000000,
    0x04090000,
    0xfc7c0000,
    0x04e2fbe6,
    0x00000000,
    0x07000000,
}};

data_ov012_02111ff8_Layout data_ov012_02111ff8 = {{
    0xfa8f00ed, 0xf9c2fb82, 0x00000000, 0x35000000, 0xfaba002a, 0xf0f6fbb4, 0x00000000, 0x05550000,
    0xf4de002a, 0xfbd2fb82, 0x80000000, 0x06550000, 0xf0c40050, 0xfaecfaba, 0x2000ffff, 0x0af30000,
    0xf15a0108, 0xfa92fc4a, 0x00000000, 0xffff0000, 0xf4ac0108, 0x019afcae, 0x00000000, 0xffff0000,
    0xf83a0108, 0xff9cfd12, 0x40000000, 0xffff0000, 0xf83a0108, 0xfd44fd12, 0x40000000, 0xffff0000,
    0xf83a0108, 0xfaecfd12, 0x40000000, 0xffff0000, 0xf9b60108,
}};
