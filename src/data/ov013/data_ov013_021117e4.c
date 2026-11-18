/* ov013 cartridge data at 0x021117e4, 8 bytes. */
#include "types.h"

extern u32 data_ov013_02111e18[];

u32 data_ov013_021117e4[2] = {
    0x00001000,
    (u32)&data_ov013_02111e18,
};
