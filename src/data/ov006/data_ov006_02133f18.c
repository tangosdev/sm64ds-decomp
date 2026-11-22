/* claimed-storage */
/* ov006 .data.gap5 0x02133f18..0x02133f24. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02133f1c[];

#pragma define_section claimsec ".data.gap5" ".data.gap5" ".data.gap5" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sMCarloHudSprite[] = {
    (u32)&data_ov006_02133f1c,
    0x81f880f0, 0xffff0100,
};
#pragma explicit_zero_data off
#pragma section claimsec end
