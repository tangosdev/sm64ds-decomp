/* ov013 cartridge data at 0x021117cc, 8 bytes. */
#include "types.h"

extern u32 data_ov013_021117e4[];

u32 data_ov013_021117cc[2] = {
    0x00000001,
    (u32)&data_ov013_021117e4,
};
