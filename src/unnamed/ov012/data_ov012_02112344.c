#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
} _ZTV16daObjC0_Switch_c_Layout;
typedef char _ZTV16daObjC0_Switch_c_size[sizeof(_ZTV16daObjC0_Switch_c_Layout) == 0x14 ? 1 : -1];

extern _ZTV16daObjC0_Switch_c_Layout _ZTV16daObjC0_Switch_c;
extern u8 _ZN16daObjC0_Switch_c13InitResourcesEv[];
extern u8 _ZN16daObjC0_Switch_c16CleanupResourcesEv[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

_ZTV16daObjC0_Switch_c_Layout _ZTV16daObjC0_Switch_c = {
    &_ZN16daObjC0_Switch_c13InitResourcesEv[0],    0x02011268, 0x02011244,
    &_ZN16daObjC0_Switch_c16CleanupResourcesEv[0], 0x02011220,
};
