/* ov016 cartridge data at 0x02114ba8, 36 bytes. */
#include "types.h"

struct daObjKi_Ita_c;
extern struct daObjKi_Ita_c *_Z23daObjKi_Ita_c_classInitv(void);
extern u32 _ZTI13daObjKi_Ita_c[];

#pragma define_section g_profile_KI_ITA ".data.morayTail"                                          \
                                        ".data.morayTail"                                          \
                                        ".data.morayTail" far_data
#pragma section g_profile_KI_ITA begin
u32 g_profile_KI_ITA[9] = {
    (u32)&_Z23daObjKi_Ita_c_classInitv,
    0x00b8003c,
    0x00000002,
    0x00000000,
    0x00250000,
    0x02000000,
    0x00000000,
    0x00000000,
    (u32)&_ZTI13daObjKi_Ita_c,
};
#pragma section g_profile_KI_ITA end
