#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov035_02112358_Layout;
typedef char data_ov035_02112358_size[sizeof(data_ov035_02112358_Layout) == 0x10 ? 1 : -1];

extern data_ov035_02112358_Layout data_ov035_02112358;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112358_Layout data_ov035_02112358 = {{
    0x20000000,
    0xff020000,
    0xef4c0578,
    0x00000320,
}};
