/* ov036 cartridge data at 0x02112a10, 8 bytes. */
#include "types.h"

extern u32 data_ov036_02112c94[];

u32 data_ov036_02112a10[2] = {
    0x00000008,
    (u32)&data_ov036_02112c94,
};
