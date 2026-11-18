/* ov053 cartridge data at 0x0211239c, 8 bytes. */
#include "types.h"

extern u32 data_ov053_0211238c[];

u32 data_ov053_0211239c[2] = {
    0x00000001,
    (u32)&data_ov053_0211238c,
};
