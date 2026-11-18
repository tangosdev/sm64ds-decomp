/* ov034 cartridge data at 0x02114094, 8 bytes. */
#include "types.h"

extern u32 data_ov034_02114114[];

u32 data_ov034_02114094[2] = {
    0x00000007,
    (u32)&data_ov034_02114114,
};
