/* claimed-storage */
/* ov002 .rodata 0x020ff0a0..0x020ff0b8. 2 storage addresses, one object. */
#include "types.h"

extern u32 data_ov002_0210de48[];
extern u32 data_ov002_0210de60[];
extern u32 data_ov002_0210de50[];
extern u32 data_ov002_0210da40[];
extern u32 data_ov002_0210d9a0[];
extern u32 data_ov002_0210d9c0[];

#pragma explicit_zero_data on
const u32 data_ov002_020ff0a0[] = {
    (u32)&data_ov002_0210de48,
    (u32)&data_ov002_0210de60,
    (u32)&data_ov002_0210de50,
    (u32)&data_ov002_0210da40,
    (u32)&data_ov002_0210d9a0,
    (u32)&data_ov002_0210d9c0,
};
#pragma explicit_zero_data off
