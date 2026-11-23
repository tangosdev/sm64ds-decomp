#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} g_profile_SL_ICEBLOCK_SHOT_Layout;
typedef char
    g_profile_SL_ICEBLOCK_SHOT_size[sizeof(g_profile_SL_ICEBLOCK_SHOT_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[7];
} data_ov027_02113804_Layout;
typedef char data_ov027_02113804_size[sizeof(data_ov027_02113804_Layout) == 0x1c ? 1 : -1];

extern g_profile_SL_ICEBLOCK_SHOT_Layout g_profile_SL_ICEBLOCK_SHOT;
extern data_ov027_02113804_Layout data_ov027_02113804;
extern u8 _Z44daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOTv[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

g_profile_SL_ICEBLOCK_SHOT_Layout g_profile_SL_ICEBLOCK_SHOT = {
    &_Z44daObjSlIceBlock_c_classInit_SL_ICEBLOCK_SHOTv[0],
};

data_ov027_02113804_Layout data_ov027_02113804 = {{
    0x0108005d,
    0x00000002,
    0x00060000,
    0x00500000,
    0x01500000,
    0x00000000,
    0x00000000,
}};
