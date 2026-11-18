/* ov032 cartridge data at 0x02113040, 64 bytes. */
#include "types.h"

extern u32 data_ov032_02112ec0[];
extern u32 data_ov032_02112ec4[];
extern u32 data_ov032_02112ec8[];
extern u32 data_ov032_02112f4c[];
extern u32 data_ov032_02113080[];
extern u32 data_ov032_021130c8[];
extern u32 data_ov032_02113148[];
extern u32 data_ov032_021131c8[];

u32 data_ov032_02113040[16] = {
    0x00001b02, (u32)&data_ov032_021131c8, 0x00000403, (u32)&data_ov032_02112f4c,
    0x00000904, (u32)&data_ov032_021130c8, 0x0000010b, (u32)&data_ov032_02112ec4,
    0x00000801, (u32)&data_ov032_02113148, 0x0000010c, (u32)&data_ov032_02112ec0,
    0x0000010e, (u32)&data_ov032_02112ec8, 0x0000050a, (u32)&data_ov032_02113080,
};
