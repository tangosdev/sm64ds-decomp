/* claimed-storage */
/* ov007 .rodata.gap21 0x020ce6cc..0x020ce704. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov007_020d7e18[];
extern u32 data_ov007_020d7e20[];

#pragma define_section claimsec ".rodata.gap21" ".rodata.gap21" ".rodata.gap21" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov007_020ce6cc[] = {
    0x0000ffff,
    (u32)&data_ov007_020d7e18,
    0x00000001, 0x00000001, 0x00000001, 0x00000030, 0x00300030, 0x0000ffff,
    (u32)&data_ov007_020d7e20,
    0x00000001, 0x00000001, 0x00010001, 0x00600030, 0x00900030,
};
#pragma explicit_zero_data off
#pragma section claimsec end
