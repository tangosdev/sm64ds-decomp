#include "types.h"

extern u32 data_ov013_021117bc[];
extern u32 data_ov013_021117cc[];
extern u32 data_ov013_02111794[];
extern u32 data_ov013_021117d4[];

u32 data_ov013_0211192c[6] = {
    0x0b9f0301, (u32)&data_ov013_021117bc, 0x00000000,
    0x0c8f0202, (u32)&data_ov013_021117cc, 0x00000000,
};

u32 data_ov013_02111944[7] = {
    0x08e30203, (u32)&data_ov013_02111794, 0x00000000,
    0x11300304, (u32)&data_ov013_021117d4, 0x00000000,
    0x16440105,
};
