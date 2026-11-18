/* ov009 cartridge data at 0x02112238, 28 bytes. */
#include "types.h"

extern char data_ov009_02112b7c[];

const u32 data_ov009_02112238[7] = {
    0x0000ffff, (u32)&data_ov009_02112b7c, 0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x0000005b,
};
