/* ov009 cartridge data at 0x02113abc, 36 bytes. */
#include "types.h"

extern void _Z28daObjMc_Metalnet_c_classInitv(void);
extern u32 _ZTI18daObjMc_Metalnet_c[];

u32 g_profile_MC_METALNET[9] = {
    (u32)&_Z28daObjMc_Metalnet_c_classInitv, 0x01530153, 0x00000002, 0x00000000, 0x00500000, 0x01000000,
    0x00000000, 0x00000000, (u32)&_ZTI18daObjMc_Metalnet_c,
};
