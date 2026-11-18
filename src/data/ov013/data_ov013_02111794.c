/* ov013 cartridge data at 0x02111794, 8 bytes. */
#include "types.h"

extern u32 data_ov013_02111824[];

u32 data_ov013_02111794[2] = {
    0x00000002,
    (u32)&data_ov013_02111824,
};
