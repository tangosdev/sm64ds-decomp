#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    u32 f00c;
} data_ov012_02112358_Layout;
typedef char data_ov012_02112358_size[sizeof(data_ov012_02112358_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
    void *f01c;
    void *f020;
    u32 f024;
    u32 f028;
    u32 f02c;
    void *f030;
    u32 f034;
    u32 f038;
    u32 f03c;
    u32 f040;
    u32 f044;
    u32 f048;
    u32 f04c;
    u32 f050;
    u32 f054;
    u32 f058;
} data_ov012_02112368_Layout;
typedef char data_ov012_02112368_size[sizeof(data_ov012_02112368_Layout) == 0x5c ? 1 : -1];

extern data_ov012_02112358_Layout data_ov012_02112358;
extern data_ov012_02112368_Layout data_ov012_02112368;
extern u8 _ZN16daObjC0_Switch_c8BehaviorEv[];
extern u8 _ZN16daObjC0_Switch_c6RenderEv[];
extern u8 _ZN16daObjC0_Switch_cD1Ev[];
extern u8 _ZN16daObjC0_Switch_cD0Ev[];
extern u8 _ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

data_ov012_02112358_Layout data_ov012_02112358 = {
    0x02011214,
    &_ZN16daObjC0_Switch_c8BehaviorEv[0],
    0x02010fd4,
    0x02010fc8,
};

data_ov012_02112368_Layout data_ov012_02112368 = {
    &_ZN16daObjC0_Switch_c6RenderEv[0],
    0x02010f78,
    0x02010f6c,
    0x02043ac0,
    0x0204357c,
    0x0204349c,
    0x02043494,
    &_ZN16daObjC0_Switch_cD1Ev[0],
    &_ZN16daObjC0_Switch_cD0Ev[0],
    0x02010160,
    0x02010154,
    0x0201014c,
    &_ZN16daObjC0_Switch_c15OnGroundPoundedER8dActor_c[0],
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
