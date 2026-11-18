/* ov009 cartridge data at 0x02113934, 36 bytes. */
#include "types.h"

extern void _Z19daSBird_c_classInitv(void);
extern u32 _ZTI9daSBird_c[];

u32 g_profile_SBIRD[9] = {
    (u32)&_Z19daSBird_c_classInitv, 0x01570157, 0x00010002, 0x00000000, 0x00028000, 0x01000000,
    0x00320000, 0x00000000, (u32)&_ZTI9daSBird_c,
};
