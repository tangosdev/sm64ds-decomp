/* ov053 cartridge data at 0x0211238c, 8 bytes. */
#include "types.h"

extern u32 data_ov053_021123d4[];

u32 data_ov053_0211238c[2] = {
    0x00000100,
    (u32)&data_ov053_021123d4,
};
