#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov009_02112e7c[];
extern u32 data_ov009_02112b74[];

#pragma define_section s ".data.data_ov009_02112bdc" ".data.data_ov009_02112bdc" ".data.data_ov009_02112bdc" far_data
#pragma section s begin
u32 data_ov009_02112bdc[4] = {
    (u32)&data_ov009_02112e7c,
    (u32)&data_ov009_02112b74,
    0x07950797,
    0x07990798,
};

#pragma section s end
