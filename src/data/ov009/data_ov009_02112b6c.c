/* ov009 cartridge data at 0x02112b6c, 8 bytes. */
#include "types.h"

extern u32 data_ov009_02112c9c[];

u32 data_ov009_02112b6c[2] = {
    0x00000006,
    (u32)&data_ov009_02112c9c,
};
