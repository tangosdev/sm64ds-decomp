/* claimed-storage */
/* ov007 .rodata.gap24 0x020ce8c4..0x020ce8fc. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov007_020d7e18[];
extern u32 data_ov007_020d7e20[];

#pragma define_section claimsec ".rodata.gap24" ".rodata.gap24" ".rodata.gap24" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov007_020ce8c4[] = {
    0x0000ffff,
    (u32)&data_ov007_020d7e18,
    0x00000001, 0x00000001, 0x00000001, 0x000002bc, 0x02bc02bc, 0x0000ffff,
    (u32)&data_ov007_020d7e20,
    0x00000001, 0x00000001, 0x00000001, 0x057802bc, 0x083402bc,
};
#pragma explicit_zero_data off
#pragma section claimsec end
