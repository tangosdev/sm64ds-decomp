/* ov053 cartridge data at 0x0211244c, 24 bytes. */
#include "types.h"

extern u32 data_ov053_021111a8[];
extern u32 data_ov053_021111d0[];
extern u32 data_ov053_02111234[];
extern u32 data_ov053_021115ec[];

u32 data_ov053_0211244c[6] = {
    0x0000002c, (u32)&data_ov053_021111d0, (u32)&data_ov053_021111a8, (u32)&data_ov053_021115ec,
    0x00000002, (u32)&data_ov053_02111234,
};
