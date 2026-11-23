#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov035_02112278_Layout;
typedef char data_ov035_02112278_size[sizeof(data_ov035_02112278_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    void *f008;
    u32 f00c;
    void *f010;
    u32 f014;
} data_ov035_0211227c_Layout;
typedef char data_ov035_0211227c_size[sizeof(data_ov035_0211227c_Layout) == 0x18 ? 1 : -1];

extern data_ov035_02112278_Layout data_ov035_02112278;
extern data_ov035_0211227c_Layout data_ov035_0211227c;
extern u8 data_ov035_02112318[];
extern u8 data_ov035_02112044[];
extern u8 data_ov035_021122d8[];

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

data_ov035_02112278_Layout data_ov035_02112278 = {{
    0x00000604,
}};

data_ov035_0211227c_Layout data_ov035_0211227c = {
    &data_ov035_02112318[0], 0x0000010b, &data_ov035_02112044[0], 0x00000401,
    &data_ov035_021122d8[0], 0x00000108,
};
