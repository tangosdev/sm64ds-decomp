/* ov016 cartridge data at 0x021148e0, 16 bytes. */
#include "types.h"

struct daMoray_c;
extern struct daMoray_c *_Z19daMoray_c_classInitv(void);

u32 g_profile_MORAY[4] = {
    (u32)&_Z19daMoray_c_classInitv,
    0x006200f2,
    0x00000000,
    0x00000000,
};
