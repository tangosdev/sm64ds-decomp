/* claimed-storage */
/* ov006 .data.gap6 0x02133fb0..0x02134028. 7 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02134000[];
extern u32 data_ov006_02133ff0[];
extern u32 data_ov006_02133fe0[];
extern u32 data_ov006_02133fd0[];
extern u32 data_ov006_02133fc0[];

#pragma define_section claimsec ".data.gap6" ".data.gap6" ".data.gap6" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_02133fb0[] = {
    0x81e040f8, 0x00000140, 0x800040f8, 0xffff0144, 0x81e040f8, 0x00000100, 0x800040f8, 0xffff0104,
    0x81e040f8, 0x000000c0, 0x800040f8, 0xffff00c4, 0x81e040f8, 0x00000080, 0x800040f8, 0xffff0084,
    0x81e040f8, 0x00000040, 0x800040f8, 0xffff0044, 0x81e040f8, 0x00000000, 0x800040f8, 0xffff0004,
    (u32)&data_ov006_02134000,
    (u32)&data_ov006_02133ff0,
    (u32)&data_ov006_02133fe0,
    (u32)&data_ov006_02133fd0,
    (u32)&data_ov006_02133fc0,
    (u32)&data_ov006_02133fb0,
};
#pragma explicit_zero_data off
#pragma section claimsec end
