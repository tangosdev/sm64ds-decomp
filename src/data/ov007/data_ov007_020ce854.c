/* claimed-storage */
/* ov007 .rodata.gap23 0x020ce854..0x020ce88c. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov007_020d7e18[];
extern u32 data_ov007_020d7e20[];

#pragma define_section claimsec ".rodata.gap23" ".rodata.gap23" ".rodata.gap23" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov007_020ce854[] = {
    0x0000ffff,
    (u32)&data_ov007_020d7e18,
    0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x0000ffff,
    (u32)&data_ov007_020d7e20,
    0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00000001,
};
#pragma explicit_zero_data off
#pragma section claimsec end
