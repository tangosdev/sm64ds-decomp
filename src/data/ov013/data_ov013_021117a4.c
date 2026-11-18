/* ov013 cartridge data at 0x021117a4, 8 bytes. */
#include "types.h"

extern u32 data_ov013_021118d0[];

u32 data_ov013_021117a4[2] = {
    0x00000009,
    (u32)&data_ov013_021118d0,
};
