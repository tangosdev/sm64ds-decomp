#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov055_02111a78_Layout;
typedef char data_ov055_02111a78_size[sizeof(data_ov055_02111a78_Layout) == 0x18 ? 1 : -1];

extern data_ov055_02111a78_Layout data_ov055_02111a78;

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111a78_Layout data_ov055_02111a78 = {{
    0xfe0ce225,
    0x05460000,
    0xfed4e225,
    0xfaba0000,
    0xfed4e225,
    0x05460000,
}};
