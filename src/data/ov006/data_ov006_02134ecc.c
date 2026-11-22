/* claimed-storage */
/* ov006 .data.gap15b 0x02134ecc..0x02134ef0. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02134ed0[];

#pragma define_section claimsec ".data.gap15b" ".data.gap15b" ".data.gap15b" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sTrmpLifeIconSprite[] = {
    (u32)&data_ov006_02134ed0,
    0x41f100f4, 0x00002049, 0x000180f4, 0x0000204b, 0x01f14004, 0x00002046, 0x00010004, 0xffff2048,
};
#pragma explicit_zero_data off
#pragma section claimsec end
