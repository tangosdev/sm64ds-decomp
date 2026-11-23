#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov035_02112368_Layout;
typedef char data_ov035_02112368_size[sizeof(data_ov035_02112368_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[30];
} data_ov035_0211236c_Layout;
typedef char data_ov035_0211236c_size[sizeof(data_ov035_0211236c_Layout) == 0x78 ? 1 : -1];

extern data_ov035_02112368_Layout data_ov035_02112368;
extern data_ov035_0211236c_Layout data_ov035_0211236c;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112368_Layout data_ov035_02112368 = {{
    0x00008000,
}};

data_ov035_0211236c_Layout data_ov035_0211236c = {{
    0x53504c43, 0x000c0008, 0x00230000, 0x000000ff, 0x008b200c, 0x000000ff, 0x008b000c, 0x000000ff,
    0x018b000c, 0x000000ff, 0x0103000c, 0x000000ff, 0x00000fc5, 0x000000ff, 0x01030005, 0x000000ff,
    0x00052045, 0x000000ff, 0x00032fc5, 0x000000ff, 0x00032005, 0x000000ff, 0x000320c5, 0x000000ff,
    0x00032105, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
}};
