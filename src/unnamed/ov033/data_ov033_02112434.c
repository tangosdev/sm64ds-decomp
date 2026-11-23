#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov033_02112434_Layout;
typedef char data_ov033_02112434_size[sizeof(data_ov033_02112434_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 f000;
    void *f004;
} data_ov033_02112438_Layout;
typedef char data_ov033_02112438_size[sizeof(data_ov033_02112438_Layout) == 0x8 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} _ZTV14daObjTtWater_c_Layout;
typedef char _ZTV14daObjTtWater_c_size[sizeof(_ZTV14daObjTtWater_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    void *f00c;
    u32 f010;
    u32 f014;
} data_ov033_0211244c_Layout;
typedef char data_ov033_0211244c_size[sizeof(data_ov033_0211244c_Layout) == 0x18 ? 1 : -1];

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
} data_ov033_02112464_Layout;
typedef char data_ov033_02112464_size[sizeof(data_ov033_02112464_Layout) == 0x30 ? 1 : -1];

extern data_ov033_02112434_Layout data_ov033_02112434;
extern data_ov033_02112438_Layout data_ov033_02112438;
extern _ZTV14daObjTtWater_c_Layout _ZTV14daObjTtWater_c;
extern data_ov033_0211244c_Layout data_ov033_0211244c;
extern data_ov033_02112464_Layout data_ov033_02112464;
extern u8 _ZTI14daObjTtWater_c[];
extern u8 _ZN14daObjTtWater_c13InitResourcesEv[];
extern u8 _ZN14daObjTtWater_c16CleanupResourcesEv[];
extern u8 _ZN14daObjTtWater_c8BehaviorEv[];
extern u8 _ZN14daObjTtWater_c6RenderEv[];
extern u8 _ZN14daObjTtWater_cD1Ev[];
extern u8 _ZN14daObjTtWater_cD0Ev[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

data_ov033_02112434_Layout data_ov033_02112434 = {{
    0x00640000,
}};

data_ov033_02112438_Layout data_ov033_02112438 = {
    0x00000000,
    &_ZTI14daObjTtWater_c[0],
};

_ZTV14daObjTtWater_c_Layout _ZTV14daObjTtWater_c = {
    &_ZN14daObjTtWater_c13InitResourcesEv[0],
    0x02011268,
    0x02011244,
};

data_ov033_0211244c_Layout data_ov033_0211244c = {
    &_ZN14daObjTtWater_c16CleanupResourcesEv[0], 0x02011220, 0x02011214,
    &_ZN14daObjTtWater_c8BehaviorEv[0],          0x02010fd4, 0x02010fc8,
};

data_ov033_02112464_Layout data_ov033_02112464 = {
    &_ZN14daObjTtWater_c6RenderEv[0],
    0x02010f78,
    0x02010f6c,
    0x02043ac0,
    0x0204357c,
    0x0204349c,
    0x02043494,
    &_ZN14daObjTtWater_cD1Ev[0],
    &_ZN14daObjTtWater_cD0Ev[0],
    0x02010160,
    0x02010154,
    0x0201014c,
};
