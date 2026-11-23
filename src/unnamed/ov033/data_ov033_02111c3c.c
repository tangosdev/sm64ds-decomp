#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[6];
} data_ov033_02111c3c_Layout;
typedef char data_ov033_02111c3c_size[sizeof(data_ov033_02111c3c_Layout) == 0x18 ? 1 : -1];

extern data_ov033_02111c3c_Layout data_ov033_02111c3c;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02111c3c_Layout data_ov033_02111c3c = {{
    0xfddaf934,
    0xc000f93e,
    0x05180000,
    0xfc2000ff,
    0xfba500be,
    0x0000c000,
}};
