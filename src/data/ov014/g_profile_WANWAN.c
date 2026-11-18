/* ov014 cartridge data at 0x02114750, 24 bytes. */
#include "types.h"

struct daWanwan_c;
extern struct daWanwan_c *_Z20daWanwan_c_classInitv(void);

#pragma define_section g_profile_WANWAN ".data.gap2"                                               \
                                        ".data.gap2"                                               \
                                        ".data.gap2" far_data
#pragma section g_profile_WANWAN begin
u32 g_profile_WANWAN[6] = {
    (u32)&_Z20daWanwan_c_classInitv, 0x004300db, 0x10000003, 0x00120000, 0x00400000, 0x01000000,
};
#pragma section g_profile_WANWAN end
