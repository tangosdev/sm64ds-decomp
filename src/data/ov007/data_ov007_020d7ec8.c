/* claimed-storage */
/* ov007 .data.gap1 0x020d7ec8..0x020d7ee8. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov007_020e08f0[];
extern u32 data_ov007_020dd6c8[];
extern u32 data_ov007_020defdc[];
extern u32 data_ov007_020dbdbc[];

#pragma define_section claimsec ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov007_020d7ec8[] = {
    0x00000000,
    (u32)&data_ov007_020e08f0,
    (u32)&data_ov007_020dd6c8,
    (u32)&data_ov007_020defdc,
    (u32)&data_ov007_020dbdbc,
    0x00000000, 0x00000000, 0x00000000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
