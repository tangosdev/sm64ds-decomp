/* ov032 cartridge data at 0x02112ee8, 12 bytes. */
#include "types.h"

extern u32 data_ov032_02112ecc[];
extern u32 data_ov032_02112f34[];

#pragma define_section data_ov032_02112ee8 ".data.w12ee8" ".data.w12ee8" ".data.w12ee8" far_data
#pragma section data_ov032_02112ee8 begin
u32 data_ov032_02112ee8[3] = {
    (u32)&data_ov032_02112ecc, (u32)&data_ov032_02112f34, 0x1b580000,
};
#pragma section data_ov032_02112ee8 end
