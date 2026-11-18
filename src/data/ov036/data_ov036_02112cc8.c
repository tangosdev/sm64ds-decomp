/* ov036 cartridge data at 0x02112cc8, 8 bytes. */
#include "types.h"

extern u32 data_ov036_02112a34[];

u32 data_ov036_02112cc8[2] = {
    (u32)&data_ov036_02112a34,
    0x0000010e,
};
