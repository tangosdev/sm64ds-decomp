/* claimed-storage */
/* ov006 .rodata.gap9 0x0212ecec..0x0212ed14. 2 storage addresses, one object. */
#include "types.h"

#pragma define_section claimsec ".rodata.gap9" ".rodata.gap9" ".rodata.gap9" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sPanelShakeOffsets[] = {
    0x00000002, 0xfffffffe, 0x00000002, 0xfffffffe, 0x00000002, 0x00000001, 0xffffffff, 0x00000001,
    0xffffffff, 0x00000001,
};
#pragma explicit_zero_data off
#pragma section claimsec end
