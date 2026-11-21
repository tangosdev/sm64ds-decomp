#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov036_02112a08[];

#pragma define_section s ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" far_data
#pragma section s begin
u32 data_ov036_02112a18[3] = {
    (u32)&data_ov036_02112a08,
    0x00000000,
    0x1b7b0000,
};

u32 data_ov036_02112a24[4] = {
    0x0ce4effc,
    0x1001f542,
    0x0905e50c,
    0x0001ff9c,
};

#pragma section s end
