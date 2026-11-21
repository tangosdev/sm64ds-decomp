#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

/* A class section sits between this run and .data, so the objects share one named section. */

#pragma define_section s ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" far_data
#pragma section s begin
u32 data_ov009_02112b64[1] = {
    0x00000796,
};

u32 data_ov009_02112b60[1] = {
    0x00000bea,
};

u32 data_ov009_02112b68[1] = {
    0x00000004,
};

#pragma section s end
