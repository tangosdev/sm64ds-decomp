#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_02112520[];
extern u32 data_ov010_02112470[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_02112278[1] = {
    0x00000600,
};

u32 data_ov010_0211227c[3] = {
    (u32)data_ov010_02112520,
    0x00000a05,
    (u32)data_ov010_02112470,
};

#pragma section s end
