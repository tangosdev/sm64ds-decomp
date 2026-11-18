/* ov034 cartridge data at 0x02114498, 36 bytes. */
#include "types.h"

extern void _Z22daHanachan_c_classInitv(void);
extern u32 _ZTI12daHanachan_c[];

u32 g_profile_HANACHAN[9] = {
    (u32)&_Z22daHanachan_c_classInitv, 0x006a00f8, 0x00000006, 0x00100000, 0x00800000, 0x01000000,
    0x01000000, 0x00000000, (u32)&_ZTI12daHanachan_c,
};
