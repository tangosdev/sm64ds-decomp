/* claimed-storage */
/* ov032 .data.w12ee8 0x02112ee8..0x02112f64. 9 storage addresses, one object. */
#include "types.h"

extern u32 data_ov032_02112ecc[];
extern u32 data_ov032_02112f34[];
extern u32 data_ov032_021128f8[];
extern u32 data_ov032_021128f4[];
extern u32 data_ov032_02112938[];
extern u32 data_ov032_02112900[];

#pragma define_section claimsec ".data.w12ee8" ".data.w12ee8" ".data.w12ee8" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov032_02112ee8[] = {
    (u32)&data_ov032_02112ecc,
    (u32)&data_ov032_02112f34,
    0x1b580000, 0xe63800ef, 0x0384f736, 0x00000000, 0xffff0000, 0xe63800ef, 0x0384f736, 0x00000000,
    0xffff0000, 0xe63800ef, 0x0384f736, 0x00000000, 0xffff0000, 0xe63800ef, 0x0384f736, 0x00000000,
    0xffff0000, 0x0000005b,
    (u32)&data_ov032_021128f8,
    (u32)&data_ov032_021128f4,
    (u32)&data_ov032_02112938,
    0x00000001,
    (u32)&data_ov032_02112900,
    0xff080000, 0x00087fff, 0x7fffff0f, 0xff020017, 0x00197f01, 0x7f00ff02,
};
#pragma explicit_zero_data off
#pragma section claimsec end
