#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" far_data
#pragma section s begin
u32 data_ov036_02112a00[1] = {
    0x000007aa,
};

u32 data_ov036_02112a04[1] = {
    0x11111114,
};

#pragma section s end
