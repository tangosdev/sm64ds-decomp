#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} data_ov055_02111a30_Layout;
typedef char data_ov055_02111a30_size[sizeof(data_ov055_02111a30_Layout) == 0x1c ? 1 : -1];

extern data_ov055_02111a30_Layout data_ov055_02111a30;

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111a30_Layout data_ov055_02111a30 = {{
    0xfaece225,
    0xfaba0000,
    0xfbb4e225,
    0xfaba0000,
    0xfaece225,
    0x05460000,
    0xfbb4e225,
}};
