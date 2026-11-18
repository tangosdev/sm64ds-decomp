/* ov009 cartridge data at 0x02112bac, 24 bytes. */
#include "types.h"

extern u32 data_ov009_02112228[];
extern u32 data_ov009_02112230[];
extern u32 data_ov009_02112254[];
extern u32 data_ov009_02112270[];

u32 data_ov009_02112bac[6] = {
    0x0000001f, (u32)&data_ov009_02112230, (u32)&data_ov009_02112228, (u32)&data_ov009_02112270,
    0x00000001, (u32)&data_ov009_02112254,
};
