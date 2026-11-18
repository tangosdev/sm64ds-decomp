/* ov053 cartridge data at 0x02112438, 20 bytes. */
#include "types.h"

extern u32 data_ov053_021111b8[];
extern u32 data_ov053_021111d4[];
extern u32 data_ov053_021111fc[];
extern u32 data_ov053_02112098[];

u32 data_ov053_02112438[5] = {
    (u32)&data_ov053_021111d4, (u32)&data_ov053_021111b8, (u32)&data_ov053_02112098, 0x00000002,
    (u32)&data_ov053_021111fc,
};
