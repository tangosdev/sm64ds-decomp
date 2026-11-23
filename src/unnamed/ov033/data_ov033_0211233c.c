#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} _ZTI13daObjTtFuta_c_Layout;
typedef char _ZTI13daObjTtFuta_c_size[sizeof(_ZTI13daObjTtFuta_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[4];
} _ZTS13daObjTtFuta_c_Layout;
typedef char _ZTS13daObjTtFuta_c_size[sizeof(_ZTS13daObjTtFuta_c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
} g_profile_TT_FUTA_Layout;
typedef char g_profile_TT_FUTA_size[sizeof(g_profile_TT_FUTA_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    void *f010;
} data_ov033_02112368_Layout;
typedef char data_ov033_02112368_size[sizeof(data_ov033_02112368_Layout) == 0x14 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
    void *f018;
    u32 f01c;
    u32 f020;
    void *f024;
    u32 f028;
    u32 f02c;
    u32 f030;
    u32 f034;
    u32 f038;
    u32 f03c;
    void *f040;
    void *f044;
    u32 f048;
    u32 f04c;
    u32 f050;
    void *f054;
    u32 f058;
    u32 f05c;
    u32 f060;
    u32 f064;
} _ZTV13daObjTtFuta_c_Layout;
typedef char _ZTV13daObjTtFuta_c_size[sizeof(_ZTV13daObjTtFuta_c_Layout) == 0x68 ? 1 : -1];

extern _ZTI13daObjTtFuta_c_Layout _ZTI13daObjTtFuta_c;
extern _ZTS13daObjTtFuta_c_Layout _ZTS13daObjTtFuta_c;
extern g_profile_TT_FUTA_Layout g_profile_TT_FUTA;
extern data_ov033_02112368_Layout data_ov033_02112368;
extern _ZTV13daObjTtFuta_c_Layout _ZTV13daObjTtFuta_c;
extern u8 _Z23daObjTtFuta_c_classInitv[];
extern u8 _ZN13daObjTtFuta_c13InitResourcesEv[];
extern u8 _ZN13daObjTtFuta_c16CleanupResourcesEv[];
extern u8 _ZN13daObjTtFuta_c8BehaviorEv[];
extern u8 _ZN13daObjTtFuta_c6RenderEv[];
extern u8 _ZN13daObjTtFuta_cD1Ev[];
extern u8 _ZN13daObjTtFuta_cD0Ev[];
extern u8 _ZN13daObjTtFuta_c15OnGroundPoundedER8dActor_c[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

_ZTI13daObjTtFuta_c_Layout _ZTI13daObjTtFuta_c = {
    0x0209a764,
    &_ZTS13daObjTtFuta_c,
    0x021089ec,
};

g_profile_TT_FUTA_Layout g_profile_TT_FUTA = {
    &_Z23daObjTtFuta_c_classInitv[0],
    0x010a0068,
    0x00000002,
    0x00000000,
};

_ZTS13daObjTtFuta_c_Layout _ZTS13daObjTtFuta_c = {{
    0x61643331,
    0x546a624f,
    0x74754674,
    0x00635f61,
}};

data_ov033_02112368_Layout data_ov033_02112368 = {
    0x00200000, 0x01000000, 0x00000000, 0x00000000, &_ZTI13daObjTtFuta_c,
};

_ZTV13daObjTtFuta_c_Layout _ZTV13daObjTtFuta_c = {
    &_ZN13daObjTtFuta_c13InitResourcesEv[0],
    0x02011268,
    0x02011244,
    &_ZN13daObjTtFuta_c16CleanupResourcesEv[0],
    0x02011220,
    0x02011214,
    &_ZN13daObjTtFuta_c8BehaviorEv[0],
    0x02010fd4,
    0x02010fc8,
    &_ZN13daObjTtFuta_c6RenderEv[0],
    0x02010f78,
    0x02010f6c,
    0x02043ac0,
    0x0204357c,
    0x0204349c,
    0x02043494,
    &_ZN13daObjTtFuta_cD1Ev[0],
    &_ZN13daObjTtFuta_cD0Ev[0],
    0x02010160,
    0x02010154,
    0x0201014c,
    &_ZN13daObjTtFuta_c15OnGroundPoundedER8dActor_c[0],
    0x02010144,
    0x02010140,
    0x0201013c,
    0x02010138,
};
