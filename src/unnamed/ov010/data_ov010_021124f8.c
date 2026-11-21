#include "types.h"

extern u32 data_ov010_02112204[];
extern u32 data_ov010_0211222c[];
extern u32 data_ov010_0211220c[];

u32 data_ov010_021124f8[1] = {
    0x01900104,
};

u32 data_ov010_021124fc[9] = {
    (u32)data_ov010_02112204, 0x00000000, 0x028a0200,
    (u32)data_ov010_0211222c, 0x00000000, 0x04b00300,
    (u32)data_ov010_0211220c, 0x00000000, 0x04b00300,
};
