#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_021122d8[];
extern u32 data_ov010_02112318[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_02112258[4] = {
    0x047f004b,
    0xf6b702d5,
    0x4000ffff,
    0x070d0000,
};

u32 data_ov010_02112268[4] = {
    0x00000200,
    (u32)data_ov010_021122d8,
    0x00000405,
    (u32)data_ov010_02112318,
};

#pragma section s end
