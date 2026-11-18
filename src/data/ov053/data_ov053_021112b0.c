/* ov053 cartridge data at 0x021112b0, 44 bytes. */
#include "types.h"

extern char data_ov053_021123bc[];

const u32 data_ov053_021112b0[11] = {
    0x00000001, 0x00000001, 0x00000001, 0x0001003f, 0x0000ffff, (u32)&data_ov053_021123bc,
    0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x0040003f,
};
