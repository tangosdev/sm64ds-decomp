/* ov053 cartridge data at 0x02112464, 24 bytes. */
#include "types.h"

extern u32 data_ov053_021111bc[];
extern u32 data_ov053_021111c0[];
extern u32 data_ov053_0211126c[];
extern u32 data_ov053_021114f8[];

u32 data_ov053_02112464[6] = {
    0x0000001e, (u32)&data_ov053_021111c0, (u32)&data_ov053_021111bc, (u32)&data_ov053_021114f8,
    0x00000002, (u32)&data_ov053_0211126c,
};
