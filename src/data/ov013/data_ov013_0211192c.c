/* ov013 cartridge data at 0x0211192c, 24 bytes. */
#include "types.h"

extern u32 data_ov013_021117bc[];
extern u32 data_ov013_021117cc[];

u32 data_ov013_0211192c[6] = {
    0x0b9f0301, (u32)&data_ov013_021117bc, 0x00000000,
    0x0c8f0202, (u32)&data_ov013_021117cc, 0x00000000,
};
