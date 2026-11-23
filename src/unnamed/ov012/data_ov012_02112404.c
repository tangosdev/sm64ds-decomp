#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov012_02112404_Layout;
typedef char data_ov012_02112404_size[sizeof(data_ov012_02112404_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
} _ZTV14daObjC0Water_c_Layout;
typedef char _ZTV14daObjC0Water_c_size[sizeof(_ZTV14daObjC0Water_c_Layout) == 0x14 ? 1 : -1];

extern data_ov012_02112404_Layout data_ov012_02112404;
extern _ZTV14daObjC0Water_c_Layout _ZTV14daObjC0Water_c;
extern u8 _ZTI14daObjC0Water_c[];
extern u8 _ZN14daObjC0Water_c13InitResourcesEv[];
extern u8 _ZN14daObjC0Water_c16CleanupResourcesEv[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02112404_Layout data_ov012_02112404 = {
    &_ZTI14daObjC0Water_c[0],
};

_ZTV14daObjC0Water_c_Layout _ZTV14daObjC0Water_c = {
    &_ZN14daObjC0Water_c13InitResourcesEv[0],    0x02011268, 0x02011244,
    &_ZN14daObjC0Water_c16CleanupResourcesEv[0], 0x02011220,
};
