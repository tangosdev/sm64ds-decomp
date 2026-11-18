/* ov036 cartridge data at 0x02112a18, 12 bytes. */
#include "types.h"

extern u32 data_ov036_02112a08[];

u32 data_ov036_02112a18[3] = {
    (u32)&data_ov036_02112a08,
    0x00000000,
    0x1b7b0000,
};
