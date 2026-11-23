#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
} _ZTV16daObjCtMecha10_c_Layout;
typedef char _ZTV16daObjCtMecha10_c_size[sizeof(_ZTV16daObjCtMecha10_c_Layout) == 0x8 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
    void *f010;
    u32 f014;
    u32 f018;
    void *f01c;
    u32 f020;
    u32 f024;
    u32 f028;
    u32 f02c;
    u32 f030;
    u32 f034;
    void *f038;
    void *f03c;
    u32 f040;
    u32 f044;
    u32 f048;
    u32 f04c;
    u32 f050;
    u32 f054;
    u32 f058;
    u32 f05c;
    u32 f060;
    u32 f064;
    u32 f068;
    u32 f06c;
    u32 f070;
    u32 f074;
} data_ov035_02112b08_Layout;
typedef char data_ov035_02112b08_size[sizeof(data_ov035_02112b08_Layout) == 0x78 ? 1 : -1];

extern _ZTV16daObjCtMecha10_c_Layout _ZTV16daObjCtMecha10_c;
extern data_ov035_02112b08_Layout data_ov035_02112b08;
extern u8 _ZN16daObjCtMecha10_c13InitResourcesEv[];
extern u8 _ZN16daObjCtMecha10_c16CleanupResourcesEv[];
extern u8 _ZN16daObjCtMecha10_c8BehaviorEv[];
extern u8 _ZN16daObjCtMecha10_c6RenderEv[];
extern u8 _ZN16daObjCtMecha10_cD1Ev[];
extern u8 _ZN16daObjCtMecha10_cD0Ev[];

#pragma define_section sName ".data.m35Mid2"                                                       \
                             ".data.m35Mid2"                                                       \
                             ".data.m35Mid2" far_data
#pragma section sName begin

_ZTV16daObjCtMecha10_c_Layout _ZTV16daObjCtMecha10_c = {
    &_ZN16daObjCtMecha10_c13InitResourcesEv[0],
    0x02011268,
};

data_ov035_02112b08_Layout data_ov035_02112b08 = {
    0x02011244,
    &_ZN16daObjCtMecha10_c16CleanupResourcesEv[0],
    0x02011220,
    0x02011214,
    &_ZN16daObjCtMecha10_c8BehaviorEv[0],
    0x02010fd4,
    0x02010fc8,
    &_ZN16daObjCtMecha10_c6RenderEv[0],
    0x02010f78,
    0x02010f6c,
    0x02043ac0,
    0x0204357c,
    0x0204349c,
    0x02043494,
    &_ZN16daObjCtMecha10_cD1Ev[0],
    &_ZN16daObjCtMecha10_cD0Ev[0],
    0x02010160,
    0x02010154,
    0x0201014c,
    0x02010148,
    0x02010144,
    0x02010140,
    0x0201013c,
    0x02010138,
    0x02010134,
    0x02010130,
    0x0201012c,
    0x02010124,
    0x020100dc,
    0x020ee55c,
};
