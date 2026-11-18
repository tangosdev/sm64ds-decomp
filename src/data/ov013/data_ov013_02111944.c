/* ov013 cartridge data at 0x02111944, 28 bytes. */
#include "types.h"

extern u32 data_ov013_02111794[];
extern u32 data_ov013_021117d4[];

u32 data_ov013_02111944[7] = {
    0x08e30203, (u32)&data_ov013_02111794, 0x00000000,
    0x11300304, (u32)&data_ov013_021117d4, 0x00000000,
    0x16440105,
};
