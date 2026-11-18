/* ov014 cartridge data at 0x0211488c, 12 bytes. */
#include "types.h"

struct daObjWanwanShutter_c;
extern struct daObjWanwanShutter_c *_Z30daObjWanwanShutter_c_classInitv(void);

#pragma define_section g_profile_WANWAN_SHUTTER ".data.gap3"                                       \
                                                ".data.gap3"                                       \
                                                ".data.gap3" far_data
#pragma section g_profile_WANWAN_SHUTTER begin
u32 g_profile_WANWAN_SHUTTER[3] = {
    (u32)&_Z30daObjWanwanShutter_c_classInitv,
    0x00440029,
    0x00000002,
};
#pragma section g_profile_WANWAN_SHUTTER end
