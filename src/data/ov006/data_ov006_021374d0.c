/* claimed-storage */
/* ov006 .data.gap27 0x021374d0..0x021374f4. 4 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_021374e0[];
extern u32 data_ov006_021374d8[];

#pragma define_section claimsec ".data.gap27" ".data.gap27" ".data.gap27" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_021374d0[] = {
    0xc1f080e0, 0xffff5014, 0xc1f080e0, 0xffff4014, 0xc1f080e0, 0xffff0014,
    (u32)&data_ov006_021374e0,
    (u32)&data_ov006_021374d8,
    (u32)&data_ov006_021374d0,
};
#pragma explicit_zero_data off
#pragma section claimsec end
