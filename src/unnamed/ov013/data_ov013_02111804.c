#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov013_021119cc[];
extern u32 data_ov013_02111814[];
extern u32 data_ov013_02111ae4[];
extern u32 data_ov013_02111834[];

u32 data_ov013_02111824[4] = {
    0x00000700,
    (u32)&data_ov013_021119cc,
    0x00000205,
    (u32)&data_ov013_02111814,
};

u32 data_ov013_02111814[4] = {
    0xfbaf483f,
    0x0a2908ca,
    0xfa58e83c,
    0x07a10a11,
};

u32 data_ov013_02111804[4] = {
    0x00000b00,
    (u32)&data_ov013_02111ae4,
    0x00000205,
    (u32)&data_ov013_02111834,
};

u32 data_ov013_02111834[4] = {
    0xfb3f463f,
    0xfad504c9,
    0xfcc8e63c,
    0xfd8105d8,
};
