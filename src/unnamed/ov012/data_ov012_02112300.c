#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} _ZTI16daObjC0_Switch_c_Layout;
typedef char _ZTI16daObjC0_Switch_c_size[sizeof(_ZTI16daObjC0_Switch_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} _ZTS16daObjC0_Switch_c_Layout;
typedef char _ZTS16daObjC0_Switch_c_size[sizeof(_ZTS16daObjC0_Switch_c_Layout) == 0x14 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
    u32 f018;
    u32 f01c;
    void *f020;
} g_profile_C0_SWITCH_Layout;
typedef char g_profile_C0_SWITCH_size[sizeof(g_profile_C0_SWITCH_Layout) == 0x24 ? 1 : -1];

extern _ZTI16daObjC0_Switch_c_Layout _ZTI16daObjC0_Switch_c;
extern _ZTS16daObjC0_Switch_c_Layout _ZTS16daObjC0_Switch_c;
extern g_profile_C0_SWITCH_Layout g_profile_C0_SWITCH;
extern u8 _Z26daObjC0_Switch_c_classInitv[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

_ZTI16daObjC0_Switch_c_Layout _ZTI16daObjC0_Switch_c = {
    0x0209a764,
    &_ZTS16daObjC0_Switch_c,
    0x021089ec,
};

_ZTS16daObjC0_Switch_c_Layout _ZTS16daObjC0_Switch_c = {{
    0x61643631,
    0x436a624f,
    0x77535f30,
    0x68637469,
    0x0000635f,
}};

g_profile_C0_SWITCH_Layout g_profile_C0_SWITCH = {
    &_Z26daObjC0_Switch_c_classInitv[0],
    0x00e40022,
    0x00000002,
    0x00060000,
    0x00250000,
    0x01000000,
    0x00000000,
    0x00000000,
    &_ZTI16daObjC0_Switch_c,
};
