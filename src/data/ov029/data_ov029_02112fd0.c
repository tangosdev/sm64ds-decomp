/* claimed-storage */
/* ov029 .data.gap1 0x02112fd0..0x02112fec. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov029_02113204[];
extern u32 data_ov029_02112eb8[];
extern u32 data_ov029_02112fa0[];

#pragma define_section claimsec ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov029_02112fd0[] = {
    (u32)&data_ov029_02113204,
    (u32)&data_ov029_02112eb8,
    0x07ed07f0, 0x07f207f1,
    (u32)&data_ov029_02112fa0,
    0x03e80002, 0x0000008f,
};
#pragma explicit_zero_data off
#pragma section claimsec end
