#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov027_021138f0_Layout;
typedef char data_ov027_021138f0_size[sizeof(data_ov027_021138f0_Layout) == 0x4 ? 1 : -1];

extern data_ov027_021138f0_Layout data_ov027_021138f0;

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021138f0_Layout data_ov027_021138f0 = {{
    0x00bb8000,
}};
