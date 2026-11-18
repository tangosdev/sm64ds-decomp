/* ov009 cartridge data at 0x02112c9c, 12 bytes. */
#include "types.h"

extern u32 data_ov009_02112d04[];

u32 data_ov009_02112c9c[3] = {
    0x00000400,
    (u32)&data_ov009_02112d04,
    0x00001d05,
};
