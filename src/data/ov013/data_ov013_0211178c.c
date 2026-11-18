/* ov013 cartridge data at 0x0211178c, 8 bytes. */
#include "types.h"

extern u32 data_ov013_02111890[];

u32 data_ov013_0211178c[2] = {
    0x00000400,
    (u32)&data_ov013_02111890,
};
