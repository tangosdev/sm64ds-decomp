/* claimed-storage */
/* ov006 .data.gap10 0x02134218..0x0213424c. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02134274[];
extern u32 data_ov006_02134224[];
extern u32 data_ov006_02134200[];

#pragma define_section claimsec ".data.gap10" ".data.gap10" ".data.gap10" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_02134218[] = {
    (u32)&data_ov006_02134274,
    (u32)&data_ov006_02134224,
    (u32)&data_ov006_02134200,
    0x01f500f7, 0x000011ef, 0x01fc00f1, 0x000011ef, 0x01fa0000, 0x000011ef, 0x000200fd, 0x000011ef,
    0x000200f5, 0xffff11ef,
};
#pragma explicit_zero_data off
#pragma section claimsec end
