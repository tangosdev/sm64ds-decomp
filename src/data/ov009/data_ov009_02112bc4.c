/* ov009 cartridge data at 0x02112bc4, 24 bytes. */
#include "types.h"

extern u32 data_ov009_0211222c[];
extern u32 data_ov009_02112234[];
extern u32 data_ov009_02112238[];
extern u32 data_ov009_021122ec[];

u32 data_ov009_02112bc4[6] = {
    0x0000005b, (u32)&data_ov009_02112234, (u32)&data_ov009_0211222c, (u32)&data_ov009_021122ec, 0x00000001, (u32)&data_ov009_02112238,
};
