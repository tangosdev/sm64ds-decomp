/* ov053 cartridge data at 0x021112a4, 12 bytes. */
#include "types.h"

extern char data_ov053_021123b0[];

const u32 data_ov053_021112a4[3] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
};
