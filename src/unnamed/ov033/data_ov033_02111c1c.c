#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov033_02111c1c_Layout;
typedef char data_ov033_02111c1c_size[sizeof(data_ov033_02111c1c_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov033_02111c20_Layout;
typedef char data_ov033_02111c20_size[sizeof(data_ov033_02111c20_Layout) == 0x1c ? 1 : -1];

extern data_ov033_02111c1c_Layout data_ov033_02111c1c;
extern data_ov033_02111c20_Layout data_ov033_02111c20;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02111c1c_Layout data_ov033_02111c1c = {{
    0x53504c43,
}};

data_ov033_02111c20_Layout data_ov033_02111c20 = {{
    0x00010008,
    0x00000fe0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
