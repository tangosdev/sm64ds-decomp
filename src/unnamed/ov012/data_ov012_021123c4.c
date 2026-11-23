#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} _ZTI14daObjC0Water_c_Layout;
typedef char _ZTI14daObjC0Water_c_size[sizeof(_ZTI14daObjC0Water_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} _ZTS14daObjC0Water_c_Layout;
typedef char _ZTS14daObjC0Water_c_size[sizeof(_ZTS14daObjC0Water_c_Layout) == 0x14 ? 1 : -1];

extern _ZTI14daObjC0Water_c_Layout _ZTI14daObjC0Water_c;
extern _ZTS14daObjC0Water_c_Layout _ZTS14daObjC0Water_c;

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

_ZTI14daObjC0Water_c_Layout _ZTI14daObjC0Water_c = {
    0x0209a764,
    &_ZTS14daObjC0Water_c,
    0x021089ec,
};

_ZTS14daObjC0Water_c_Layout _ZTS14daObjC0Water_c = {{
    0x61643431,
    0x436a624f,
    0x74615730,
    0x635f7265,
    0x00000000,
}};
