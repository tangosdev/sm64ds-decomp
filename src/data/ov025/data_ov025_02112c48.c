/* claimed-storage */
/* ov025 .data 0x02112c48..0x02112ce8. 8 storage addresses, one object. */
#include "types.h"

extern u32 data_ov025_02113154[];
extern u32 data_ov025_02112fec[];
extern u32 data_ov025_02112b9c[];
extern u32 data_ov025_02112d68[];

#pragma explicit_zero_data on
u32 data_ov025_02112c48[] = {
    0x00002b00,
    (u32)&data_ov025_02113154,
    0x00002d05,
    (u32)&data_ov025_02112fec,
    0x00000265,
    (u32)&data_ov025_02112b9c,
    0x000006c5,
    (u32)&data_ov025_02112d68,
    0x00000117, 0xf380fa24, 0x00000000, 0x40000000, 0x02140108, 0xf14efbce, 0x00000000, 0xffff0000,
    0x53504c43, 0x00010008, 0x01000fc4, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x01000fc4, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x53504c43, 0x00010008, 0x01000fc4, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};
#pragma explicit_zero_data off
