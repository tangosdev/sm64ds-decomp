/* ov013 cartridge data at 0x02111960, 12 bytes. */
#include "types.h"

extern u32 data_ov013_021117dc[];

u32 data_ov013_02111960[3] = {
    (u32)&data_ov013_021117dc,
    0x00000000,
    0x08e30203,
};
