/* ov053 cartridge data at 0x0211241c, 12 bytes. */
#include "types.h"

extern u32 data_ov053_021111b4[];
extern u32 data_ov053_021111cc[];

u32 data_ov053_0211241c[3] = {
    0x000000b5,
    (u32)&data_ov053_021111cc,
    (u32)&data_ov053_021111b4,
};
