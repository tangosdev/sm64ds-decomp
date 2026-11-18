/* ov032 cartridge data at 0x02112f34, 24 bytes. */
#include "types.h"

extern u32 data_ov032_021128f4[];
extern u32 data_ov032_021128f8[];
extern u32 data_ov032_02112900[];
extern u32 data_ov032_02112938[];

#pragma define_section data_ov032_02112f34 ".data.w12ee8"                                          \
                                           ".data.w12ee8"                                          \
                                           ".data.w12ee8" far_data
#pragma section data_ov032_02112f34 begin
u32 data_ov032_02112f34[6] = {
    0x0000005b, (u32)&data_ov032_021128f8, (u32)&data_ov032_021128f4, (u32)&data_ov032_02112938,
    0x00000001, (u32)&data_ov032_02112900,
};
#pragma section data_ov032_02112f34 end
