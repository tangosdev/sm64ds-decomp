/* claimed-storage */
/* ov006 .data.gap14 0x02134cf8..0x02134d1c. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02134cfc[];

#pragma define_section claimsec ".data.gap14" ".data.gap14" ".data.gap14" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sJumpLifeIconSprite[] = {
    (u32)&data_ov006_02134cfc,
    0x41f100f4, 0x00001015, 0x000180f4, 0x00001017, 0x01f14004, 0x00001012, 0x00010004, 0xffff1014,
};
#pragma explicit_zero_data off
#pragma section claimsec end
