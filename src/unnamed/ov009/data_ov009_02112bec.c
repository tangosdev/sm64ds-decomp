#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov009_02112b88[];

#pragma define_section s ".data.data_ov009_02112bdc" ".data.data_ov009_02112bdc" ".data.data_ov009_02112bdc" far_data
#pragma section s begin
u32 data_ov009_02112bec[3] = {
    (u32)&data_ov009_02112b88,
    0x06400001,
    0x0000001f,
};

#pragma section s end
