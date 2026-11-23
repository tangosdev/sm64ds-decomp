#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
} g_profile_C0_WATER_Layout;
typedef char g_profile_C0_WATER_size[sizeof(g_profile_C0_WATER_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[4];
} data_ov012_021123f4_Layout;
typedef char data_ov012_021123f4_size[sizeof(data_ov012_021123f4_Layout) == 0x10 ? 1 : -1];

extern g_profile_C0_WATER_Layout g_profile_C0_WATER;
extern data_ov012_021123f4_Layout data_ov012_021123f4;
extern u8 _Z24daObjC0Water_c_classInitv[];

#pragma define_section sName ".data.c0Rest"                                                        \
                             ".data.c0Rest"                                                        \
                             ".data.c0Rest" far_data
#pragma section sName begin

g_profile_C0_WATER_Layout g_profile_C0_WATER = {
    &_Z24daObjC0Water_c_classInitv[0],
    0x00e50023,
    0x00000000,
    0x00000000,
};

data_ov012_021123f4_Layout data_ov012_021123f4 = {{
    0x01000000,
    0x00000000,
    0x00000000,
    0x00000000,
}};
