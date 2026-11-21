#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov009_02112230[];
extern u32 data_ov009_02112228[];
extern u32 data_ov009_02112270[];
extern u32 data_ov009_02112254[];

#pragma define_section s ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" far_data
#pragma section s begin
u32 data_ov009_02112ba8[1] = {
    0xf3a802d0,
};

u32 data_ov009_02112bac[6] = {
    0x0000001f,
    (u32)&data_ov009_02112230,
    (u32)&data_ov009_02112228,
    (u32)&data_ov009_02112270,
    0x00000001,
    (u32)&data_ov009_02112254,
};

#pragma section s end
