/* ov013 cartridge data at 0x021117bc, 8 bytes. */
#include "types.h"

extern u32 data_ov013_0211178c[];

u32 data_ov013_021117bc[2] = {
    0x00000001,
    (u32)&data_ov013_0211178c,
};
