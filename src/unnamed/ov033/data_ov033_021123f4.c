#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov033_021123f4_Layout;
typedef char data_ov033_021123f4_size[sizeof(data_ov033_021123f4_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} _ZTI14daObjTtWater_c_Layout;
typedef char _ZTI14daObjTtWater_c_size[sizeof(_ZTI14daObjTtWater_c_Layout) == 0x8 ? 1 : -1];

extern data_ov033_021123f4_Layout data_ov033_021123f4;
extern _ZTI14daObjTtWater_c_Layout _ZTI14daObjTtWater_c;
extern u8 _ZTS14daObjTtWater_c[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_021123f4_Layout data_ov033_021123f4 = {{
    0x020100dc,
    0x020ee55c,
}};

_ZTI14daObjTtWater_c_Layout _ZTI14daObjTtWater_c = {
    0x0209a764,
    &_ZTS14daObjTtWater_c[0],
};
