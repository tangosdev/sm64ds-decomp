/* claimed-storage */
/* ov032 .data.tdMid 0x02112f7c..0x02112f98. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov032_0211326c[];
extern u32 data_ov032_02112ed4[];
extern u32 data_ov032_02112ee8[];

#pragma define_section claimsec ".data.tdMid" ".data.tdMid" ".data.tdMid" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov032_02112f7c[] = {
    (u32)&data_ov032_0211326c,
    (u32)&data_ov032_02112ed4,
    0x07dc07de, 0x07e007df,
    (u32)&data_ov032_02112ee8,
    0x06400001, 0x0000001f,
};
#pragma explicit_zero_data off
#pragma section claimsec end
