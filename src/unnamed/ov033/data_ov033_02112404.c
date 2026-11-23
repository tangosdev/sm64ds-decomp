#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov033_02112404_Layout;
typedef char data_ov033_02112404_size[sizeof(data_ov033_02112404_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[5];
} _ZTS14daObjTtWater_c_Layout;
typedef char _ZTS14daObjTtWater_c_size[sizeof(_ZTS14daObjTtWater_c_Layout) == 0x14 ? 1 : -1];

extern data_ov033_02112404_Layout data_ov033_02112404;
extern _ZTS14daObjTtWater_c_Layout _ZTS14daObjTtWater_c;

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112404_Layout data_ov033_02112404 = {{
    0x021089ec,
}};

_ZTS14daObjTtWater_c_Layout _ZTS14daObjTtWater_c = {{
    0x61643431,
    0x546a624f,
    0x74615774,
    0x635f7265,
    0x00000000,
}};
