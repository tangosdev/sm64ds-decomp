/* ov029 cartridge data at 0x021129a4, 16 bytes. */
#include "types.h"

extern u32 data_ov029_02112ec0[];

const u32 data_ov029_021129a4[4] = {
    0x0000ffff,
    (u32)&data_ov029_02112ec0,
    0x00000001,
    0x00000001,
};
