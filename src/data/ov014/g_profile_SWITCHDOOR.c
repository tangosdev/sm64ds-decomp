/* ov014 cartridge data at 0x021145e4, 36 bytes. */
#include "types.h"

extern void _Z24daObjBSwdoor_c_classInitv(void);
extern u32 _ZTI14daObjBSwdoor_c[];

#pragma define_section g_profile_SWITCHDOOR ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section g_profile_SWITCHDOOR begin
u32 g_profile_SWITCHDOOR[9] = {
    (u32)&_Z24daObjBSwdoor_c_classInitv, 0x00890028, 0x00000003, 0x00120000, 0x00400000, 0x01000000,
    0x00000000, 0x00000000, (u32)&_ZTI14daObjBSwdoor_c,
};
#pragma section g_profile_SWITCHDOOR end
