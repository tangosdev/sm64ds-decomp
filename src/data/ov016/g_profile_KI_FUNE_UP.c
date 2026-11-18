/* ov016 cartridge data at 0x02114a18, 36 bytes. */
#include "types.h"

extern void _Z35daObjKi_Fune_c_classInit_KI_FUNE_UPv(void);
extern u32 _ZTI14daObjKi_Fune_c[];

#pragma define_section g_profile_KI_FUNE_UP ".data.morayTail" ".data.morayTail" ".data.morayTail" far_data
#pragma section g_profile_KI_FUNE_UP begin
u32 g_profile_KI_FUNE_UP[9] = {
    (u32)&_Z35daObjKi_Fune_c_classInit_KI_FUNE_UPv, 0x00b50039, 0x00000002, 0x00600000, 0x01500000, 0x03000000,
    0x00000000, 0x00000000, (u32)&_ZTI14daObjKi_Fune_c,
};
#pragma section g_profile_KI_FUNE_UP end
