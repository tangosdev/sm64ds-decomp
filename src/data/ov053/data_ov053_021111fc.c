/* ov053 cartridge data at 0x021111fc, 8 bytes. */
#include "types.h"

extern char data_ov053_021123b0[];

const u32 data_ov053_021111fc[2] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
};
