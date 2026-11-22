/* claimed-storage */
/* ov002 .rodata 0x020ff028..0x020ff040. 1 storage addresses, one object. */
#include "types.h"

extern u32 data_ov002_0210da40[];
extern u32 data_ov002_0210d9a0[];
extern u32 data_ov002_0210d9c0[];

#pragma explicit_zero_data on
const u32 data_ov002_020ff028[] = {
    (u32)&data_ov002_0210da40,
    (u32)&data_ov002_0210d9a0,
    (u32)&data_ov002_0210d9c0,
    0x00000000, 0x00000000, 0x00000000,
};
#pragma explicit_zero_data off
