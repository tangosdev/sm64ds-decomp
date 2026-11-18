/* ov016 cartridge data at 0x02113554, 28 bytes. */
#include "types.h"

extern u32 data_ov016_02113af8[];

const u32 data_ov016_02113554[7] = {
    0x0000ffff, (u32)&data_ov016_02113af8, 0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x0000005b,
};
