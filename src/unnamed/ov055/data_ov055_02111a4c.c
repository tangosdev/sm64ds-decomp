#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov055_02111a4c_Layout;
typedef char data_ov055_02111a4c_size[sizeof(data_ov055_02111a4c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[8];
} data_ov055_02111a58_Layout;
typedef char data_ov055_02111a58_size[sizeof(data_ov055_02111a58_Layout) == 0x20 ? 1 : -1];

extern data_ov055_02111a4c_Layout data_ov055_02111a4c;
extern data_ov055_02111a58_Layout data_ov055_02111a58;

#pragma define_section sName ".data.lgPre"                                                         \
                             ".data.lgPre"                                                         \
                             ".data.lgPre" far_data
#pragma section sName begin

data_ov055_02111a4c_Layout data_ov055_02111a4c = {{
    0x05460000,
    0xfc7ce225,
    0xfaba0000,
}};

data_ov055_02111a58_Layout data_ov055_02111a58 = {{
    0xfd44e225,
    0xfaba0000,
    0xfe0ce225,
    0xfaba0000,
    0xfc7ce225,
    0x05460000,
    0xfd44e225,
    0x05460000,
}};
