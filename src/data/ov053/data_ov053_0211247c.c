/* ov053 cartridge data at 0x0211247c, 24 bytes. */
#include "types.h"

extern u32 data_ov053_02112380[];
extern u32 data_ov053_021123e4[];
extern u32 data_ov053_021124dc[];

u32 data_ov053_0211247c[6] = {
    0x00000204, (u32)&data_ov053_021124dc, 0x0000010b, (u32)&data_ov053_02112380,
    0x00000101, (u32)&data_ov053_021123e4,
};
