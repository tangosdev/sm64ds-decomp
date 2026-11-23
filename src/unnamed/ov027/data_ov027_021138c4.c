#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov027_021138c4_Layout;
typedef char data_ov027_021138c4_size[sizeof(data_ov027_021138c4_Layout) == 0x4 ? 1 : -1];

typedef struct {
    u32 w[4];
} _ZTS12daIDonketu_c_Layout;
typedef char _ZTS12daIDonketu_c_size[sizeof(_ZTS12daIDonketu_c_Layout) == 0x10 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
    u32 f014;
} g_profile_ICE_DONKETU_Layout;
typedef char g_profile_ICE_DONKETU_size[sizeof(g_profile_ICE_DONKETU_Layout) == 0x18 ? 1 : -1];

extern data_ov027_021138c4_Layout data_ov027_021138c4;
extern _ZTS12daIDonketu_c_Layout _ZTS12daIDonketu_c;
extern g_profile_ICE_DONKETU_Layout g_profile_ICE_DONKETU;
extern u8 _Z22daIDonketu_c_classInitv[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

data_ov027_021138c4_Layout data_ov027_021138c4 = {{
    0x021081c0,
}};

_ZTS12daIDonketu_c_Layout _ZTS12daIDonketu_c = {{
    0x61643231,
    0x6e6f4449,
    0x7574656b,
    0x0000635f,
}};

g_profile_ICE_DONKETU_Layout g_profile_ICE_DONKETU = {
    &_Z22daIDonketu_c_classInitv[0], 0x003d00d9, 0x10000006, 0x00064000, 0x00064000, 0x01000000,
};
