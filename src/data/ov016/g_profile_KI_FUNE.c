/* ov016 cartridge data at 0x021149fc, 28 bytes. */
#include "types.h"

struct daObjKi_Fune_c;
extern struct daObjKi_Fune_c *_Z32daObjKi_Fune_c_classInit_KI_FUNEv(void);

#pragma define_section g_profile_KI_FUNE ".data.morayTail"                                         \
                                         ".data.morayTail"                                         \
                                         ".data.morayTail" far_data
#pragma section g_profile_KI_FUNE begin
u32 g_profile_KI_FUNE[7] = {
    (u32)&_Z32daObjKi_Fune_c_classInit_KI_FUNEv,
    0x00b40038,
    0x00000002,
    0x00600000,
    0x01500000,
    0x03000000,
    0x00000000,
};
#pragma section g_profile_KI_FUNE end
