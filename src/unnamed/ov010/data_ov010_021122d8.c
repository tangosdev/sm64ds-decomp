#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_021122d8[8] = {
    0xf9a60050,
    0x04de0000,
    0x600001ff,
    0x0af00000,
    0x00000112,
    0x00000000,
    0x00000000,
    0xffff0000,
};

#pragma section s end
