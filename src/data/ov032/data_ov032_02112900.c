/* claimed-storage */
/* ov032 .rodata.w2900 0x02112900..0x0211291c. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov032_02112edc[];

#pragma define_section claimsec ".rodata.w2900" ".rodata.w2900" ".rodata.w2900" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov032_02112900[] = {
    0x0000ffff,
    (u32)&data_ov032_02112edc,
    0x00000001, 0x00000001, 0x00000001, 0x0000005b, 0x0000005b,
};
#pragma explicit_zero_data off
#pragma section claimsec end
