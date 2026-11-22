/* claimed-storage */
/* ov006 .rodata.gap9b 0x0212ed88..0x0212ee0c. 7 storage addresses, one object. */
#include "types.h"

#pragma define_section claimsec ".rodata.gap9b" ".rodata.gap9b" ".rodata.gap9b" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sRouletteQuadNumbers[] = {
    0x00010000, 0x00040003, 0x00020001, 0x00050004, 0x00040003, 0x00070006, 0x00050004, 0x00080007,
    0x00070006, 0x000a0009, 0x00080007, 0x000b000a, 0x00010000, 0x00020001, 0x00040003, 0x00050004,
    0x00070006, 0x00080007, 0x000a0009, 0x000b000a, 0x00030000, 0x00040001, 0x00050002, 0x00060003,
    0x00070004, 0x00080005, 0x00090006, 0x000a0007, 0x000b0008, 0x000001f4, 0x000007d0, 0x00000fa0,
    0x00001f40,
};
#pragma explicit_zero_data off
#pragma section claimsec end
