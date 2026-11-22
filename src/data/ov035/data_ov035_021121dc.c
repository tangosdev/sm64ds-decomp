/* claimed-storage */
/* ov035 .data.m35Mid 0x021121dc..0x02112238. 3 storage addresses, one object. */
#include "types.h"

extern u32 data_ov035_021124e4[];
extern u32 data_ov035_021123ec[];
extern u32 data_ov035_0211207c[];
extern u32 data_ov035_021122a8[];

#pragma define_section claimsec ".data.m35Mid" ".data.m35Mid" ".data.m35Mid" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov035_021121dc[] = {
    0x00010008, 0x00000fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x53504c43,
    0x00010008, 0x00001fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x00005a00,
    (u32)&data_ov035_021124e4,
    0x00001f05,
    (u32)&data_ov035_021123ec,
    0x000001c0,
    (u32)&data_ov035_0211207c,
    0x000006e5,
    (u32)&data_ov035_021122a8,
};
#pragma explicit_zero_data off
#pragma section claimsec end
