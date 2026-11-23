#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov012_02112494_Layout;
typedef char data_ov012_02112494_size[sizeof(data_ov012_02112494_Layout) == 0xc ? 1 : -1];

extern data_ov012_02112494_Layout data_ov012_02112494;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

#pragma explicit_zero_data on
data_ov012_02112494_Layout data_ov012_02112494 = {0};
#pragma explicit_zero_data off
