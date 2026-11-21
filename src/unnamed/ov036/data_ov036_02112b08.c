#include "types.h"

/* Rainbow Ride CLPS block. Class data sections sit on both sides, so it cannot use bare .data. */

#pragma define_section s ".data.gap2" ".data.gap2" ".data.gap2" far_data
#pragma section s begin
u32 data_ov036_02112b08[8] = {
    0x53504c43,
    0x00010008,
    0x01000fc0,
    0x000000ff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};

#pragma section s end
