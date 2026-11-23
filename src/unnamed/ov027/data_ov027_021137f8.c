#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov027_021137f8_Layout;
typedef char data_ov027_021137f8_size[sizeof(data_ov027_021137f8_Layout) == 0x8 ? 1 : -1];

extern data_ov027_021137f8_Layout data_ov027_021137f8;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021137f8_Layout data_ov027_021137f8 = {{
    0x01500000,
    0x00000000,
}};
