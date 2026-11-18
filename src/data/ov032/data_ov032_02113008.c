/* ov032 cartridge data at 0x02113008, 56 bytes. */
#include "types.h"

extern u32 data_ov032_02112ef4[];
extern u32 data_ov032_02112f04[];
extern u32 data_ov032_02112f14[];
extern u32 data_ov032_02112f24[];
extern u32 data_ov032_02112fd8[];
extern u32 data_ov032_0211336c[];
extern u32 data_ov032_0211346c[];

u32 data_ov032_02113008[14] = {
    0x00003100, (u32)&data_ov032_0211346c, 0x00002005, (u32)&data_ov032_0211336c,
    0x00000360, (u32)&data_ov032_02112fd8, 0x00000180, (u32)&data_ov032_02112ef4,
    0x000001a0, (u32)&data_ov032_02112f04, 0x000001c0, (u32)&data_ov032_02112f14,
    0x000001e0, (u32)&data_ov032_02112f24,
};
