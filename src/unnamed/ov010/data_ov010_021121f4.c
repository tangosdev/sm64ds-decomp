#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_02112338[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_021121f4[1] = {
    0x00000200,
};

u32 data_ov010_021121f8[1] = {
    (u32)data_ov010_02112338,
};

#pragma section s end
