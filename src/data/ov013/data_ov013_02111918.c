/* ov013 cartridge data at 0x02111918, 16 bytes. */
#include "types.h"

extern u32 data_ov013_02111784[];

u32 data_ov013_02111918[4] = {
    0x00000000,
    0x00000000,
    0x06ef0100,
    (u32)&data_ov013_02111784,
};
