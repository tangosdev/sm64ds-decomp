#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
} g_profile_TT_WATER_Layout;
typedef char g_profile_TT_WATER_size[sizeof(g_profile_TT_WATER_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[3];
} data_ov033_02112428_Layout;
typedef char data_ov033_02112428_size[sizeof(data_ov033_02112428_Layout) == 0xc ? 1 : -1];

extern g_profile_TT_WATER_Layout g_profile_TT_WATER;
extern data_ov033_02112428_Layout data_ov033_02112428;
extern u8 _Z24daObjTtWater_c_classInitv[];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

g_profile_TT_WATER_Layout g_profile_TT_WATER = {
    &_Z24daObjTtWater_c_classInitv[0],
    0x010b0069,
    0x00000000,
};

data_ov033_02112428_Layout data_ov033_02112428 = {{
    0x00064000,
    0x000fa000,
    0x00c80000,
}};
