#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
} _ZTI17daObjSlIceBlock_c_Layout;
typedef char _ZTI17daObjSlIceBlock_c_size[sizeof(_ZTI17daObjSlIceBlock_c_Layout) == 0xc ? 1 : -1];

typedef struct {
    u32 w[5];
} _ZTS17daObjSlIceBlock_c_Layout;
typedef char _ZTS17daObjSlIceBlock_c_size[sizeof(_ZTS17daObjSlIceBlock_c_Layout) == 0x14 ? 1 : -1];

typedef struct {
    void *f000;
    u32 f004;
    u32 f008;
    u32 f00c;
    u32 f010;
} g_profile_SL_ICEBLOCK_Layout;
typedef char g_profile_SL_ICEBLOCK_size[sizeof(g_profile_SL_ICEBLOCK_Layout) == 0x14 ? 1 : -1];

extern _ZTI17daObjSlIceBlock_c_Layout _ZTI17daObjSlIceBlock_c;
extern _ZTS17daObjSlIceBlock_c_Layout _ZTS17daObjSlIceBlock_c;
extern g_profile_SL_ICEBLOCK_Layout g_profile_SL_ICEBLOCK;
extern u8 _Z39daObjSlIceBlock_c_classInit_SL_ICEBLOCKv[];

#pragma define_section sName ".data.gap0"                                                          \
                             ".data.gap0"                                                          \
                             ".data.gap0" far_data
#pragma section sName begin

_ZTI17daObjSlIceBlock_c_Layout _ZTI17daObjSlIceBlock_c = {
    0x0209a764,
    &_ZTS17daObjSlIceBlock_c,
    0x021089ec,
};

_ZTS17daObjSlIceBlock_c_Layout _ZTS17daObjSlIceBlock_c = {{
    0x61643731,
    0x536a624f,
    0x6563496c,
    0x636f6c42,
    0x00635f6b,
}};

g_profile_SL_ICEBLOCK_Layout g_profile_SL_ICEBLOCK = {
    &_Z39daObjSlIceBlock_c_classInit_SL_ICEBLOCKv[0],
    0x0107005c,
    0x00000002,
    0x00060000,
    0x00500000,
};
