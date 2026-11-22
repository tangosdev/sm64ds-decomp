/* claimed-storage */
/* ov007 .rodata.gap0 0x020cf024..0x020cf120. 3 storage addresses, one object. */
#include "types.h"

extern u32 data_ov007_020d7e00[];
extern u32 data_ov007_020d7e08[];
extern u32 data_ov007_020d7e10[];

#pragma define_section claimsec ".rodata.gap0" ".rodata.gap0" ".rodata.gap0" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov007_020cf024[] = {
    0x0000ffff,
    (u32)&data_ov007_020d7e00,
    0x00000354, 0x00000354, 0x00000001, 0x00000001, 0x00010001, 0x0000ffff,
    (u32)&data_ov007_020d7e08,
    0x00000001, 0x00000001, 0x00000001, 0x00020354, 0x03560354, 0x0000ffff,
    (u32)&data_ov007_020d7e10,
    0x00000001, 0x00000001, 0x00000001, 0x06aa0354, 0x09fe0354, 0x0000ffff,
    (u32)&data_ov007_020d7e00,
    0x00000001, 0x00000001, 0x00000001, 0x00000001, 0x00010001, 0x0000ffff,
    (u32)&data_ov007_020d7e08,
    0x00010001, 0x00010001, 0x00000001, 0x00020001, 0x00030001, 0x0000ffff,
    (u32)&data_ov007_020d7e10,
    0x00010001, 0x00010001, 0x00000001, 0x00040001, 0x00050001, 0x0000ffff,
    (u32)&data_ov007_020d7e00,
    0x00000029, 0x00000029, 0x00000001, 0x00000001, 0x00010001, 0x0000ffff,
    (u32)&data_ov007_020d7e08,
    0x00290001, 0x00290001, 0x00000001, 0x00020029, 0x002b0029, 0x0000ffff,
    (u32)&data_ov007_020d7e10,
    0x00290001, 0x00290001, 0x00000001, 0x00540029, 0x007d0029,
};
#pragma explicit_zero_data off
#pragma section claimsec end
