/* ov016 cartridge data at 0x02114adc, 36 bytes. */
#include "types.h"

extern void _Z26daObjKi_Hasira_c_classInitv(void);
extern u32 _ZTI16daObjKi_Hasira_c[];

#pragma define_section g_profile_KI_HASIRA ".data.morayTail" ".data.morayTail" ".data.morayTail" far_data
#pragma section g_profile_KI_HASIRA begin
u32 g_profile_KI_HASIRA[9] = {
    (u32)&_Z26daObjKi_Hasira_c_classInitv, 0x00b6003a, 0x02000002, 0x00100000, 0x00500000, 0x02000000,
    0x00000000, 0x00000000, (u32)&_ZTI16daObjKi_Hasira_c,
};
#pragma section g_profile_KI_HASIRA end
