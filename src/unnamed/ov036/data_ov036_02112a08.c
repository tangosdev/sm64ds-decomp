#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov036_02112a54[];
extern u32 data_ov036_02112c94[];

#pragma define_section s ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" far_data
#pragma section s begin
u32 data_ov036_02112a08[2] = {
    0x00000003,
    (u32)&data_ov036_02112a54,
};

u32 data_ov036_02112a10[2] = {
    0x00000008,
    (u32)&data_ov036_02112c94,
};

#pragma section s end
