#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov027_02113820_Layout;
typedef char data_ov027_02113820_size[sizeof(data_ov027_02113820_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
} _ZTV17daObjSlIceBlock_c_Layout;
typedef char _ZTV17daObjSlIceBlock_c_size[sizeof(_ZTV17daObjSlIceBlock_c_Layout) == 0x8 ? 1 : -1];

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
    void *f064;
    u32 f068;
    u32 f06c;
    u32 f070;
    u32 f074;
} data_ov027_0211382c_Layout;
typedef char data_ov027_0211382c_size[sizeof(data_ov027_0211382c_Layout) == 0x78 ? 1 : -1];

extern data_ov027_02113820_Layout data_ov027_02113820;
extern _ZTV17daObjSlIceBlock_c_Layout _ZTV17daObjSlIceBlock_c;
extern data_ov027_0211382c_Layout data_ov027_0211382c;
extern u8 _ZTI17daObjSlIceBlock_c[];
extern u8 _ZN17daObjSlIceBlock_c13InitResourcesEv[];
extern u8 _ZN17daObjSlIceBlock_c16CleanupResourcesEv[];
extern u8 _ZN17daObjSlIceBlock_c8BehaviorEv[];
extern u8 _ZN17daObjSlIceBlock_c6RenderEv[];
extern u8 _ZN17daObjSlIceBlock_cD1Ev[];
extern u8 _ZN17daObjSlIceBlock_cD0Ev[];
extern u8 _ZN17daObjSlIceBlock_c15OnHitByMegaCharER7daPly_c[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_02113820_Layout data_ov027_02113820 = {
    &_ZTI17daObjSlIceBlock_c[0],
};

_ZTV17daObjSlIceBlock_c_Layout _ZTV17daObjSlIceBlock_c = {
    &_ZN17daObjSlIceBlock_c13InitResourcesEv[0],
    0x02011268,
};

data_ov027_0211382c_Layout data_ov027_0211382c = {
    0x02011244,
    &_ZN17daObjSlIceBlock_c16CleanupResourcesEv[0],
    0x02011220,
    0x02011214,
    &_ZN17daObjSlIceBlock_c8BehaviorEv[0],
    0x02010fd4,
    0x02010fc8,
    &_ZN17daObjSlIceBlock_c6RenderEv[0],
    0x02010f78,
    0x02010f6c,
    0x02043ac0,
    0x0204357c,
    0x0204349c,
    0x02043494,
    &_ZN17daObjSlIceBlock_cD1Ev[0],
    &_ZN17daObjSlIceBlock_cD0Ev[0],
    0x02010160,
    0x02010154,
    0x0201014c,
    0x02010148,
    0x02010144,
    0x02010140,
    0x0201013c,
    0x02010138,
    0x02010134,
    &_ZN17daObjSlIceBlock_c15OnHitByMegaCharER7daPly_c[0],
    0x0201012c,
    0x02010124,
    0x020100dc,
    0x020ee55c,
};
