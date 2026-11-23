#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov035_021124f8_Layout;
typedef char data_ov035_021124f8_size[sizeof(data_ov035_021124f8_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[13];
} data_ov035_021124fc_Layout;
typedef char data_ov035_021124fc_size[sizeof(data_ov035_021124fc_Layout) == 0x34 ? 1 : -1];

typedef struct {
    u32 w[16];
} data_ov035_02112530_Layout;
typedef char data_ov035_02112530_size[sizeof(data_ov035_02112530_Layout) == 0x40 ? 1 : -1];

extern data_ov035_021124f8_Layout data_ov035_021124f8;
extern data_ov035_021124fc_Layout data_ov035_021124fc;
extern data_ov035_02112530_Layout data_ov035_02112530;

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_021124f8_Layout data_ov035_021124f8 = {{
    0xfd26effc,
}};

data_ov035_021124fc_Layout data_ov035_021124fc = {{
    0x20000000,
    0xffff0000,
    0xfafe0056,
    0x02fff1eb,
    0xe0000000,
    0xffff0000,
    0xfc120056,
    0x0413f371,
    0x20000000,
    0xffff0000,
    0xfafe0056,
    0x0526f4f7,
    0xe0000000,
}};

data_ov035_02112530_Layout data_ov035_02112530 = {{
    0xffff0000,
    0xf8d30056,
    0xfe4c06bd,
    0x40000000,
    0xffff0000,
    0x02f80056,
    0xfc1815f4,
    0x20000000,
    0xffff0000,
    0x040d0056,
    0xfd291469,
    0x20000000,
    0xffff0000,
    0xfc900058,
    0xfbb40370,
    0x60000000,
}};
