/* ov009 cartridge data at 0x02112cc8, 4 bytes. */
#include "types.h"

extern u32 data_ov009_02112d44[];

u32 data_ov009_02112cc8[1] = {
    (u32)&data_ov009_02112d44,
};
