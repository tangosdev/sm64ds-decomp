#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov035_0211241c_Layout;
typedef char data_ov035_0211241c_size[sizeof(data_ov035_0211241c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov035_02112428_Layout;
typedef char data_ov035_02112428_size[sizeof(data_ov035_02112428_Layout) == 0xc ? 1 : -1];

extern data_ov035_0211241c_Layout data_ov035_0211241c;
extern data_ov035_02112428_Layout data_ov035_02112428;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_0211241c_Layout data_ov035_0211241c = {{
    0xfac4e225,
    0x044cfc68,
    0xfb28e225,
}};

data_ov035_02112428_Layout data_ov035_02112428 = {{
    0x04b0fc68,
    0xfbb4e225,
    0x0578fccc,
}};
