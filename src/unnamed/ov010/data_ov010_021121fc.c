#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_02112224[];
extern u32 data_ov010_02112258[];
extern u32 data_ov010_021121f4[];
extern u32 data_ov010_02112420[];
extern u32 data_ov010_02112278[];
extern u32 data_ov010_021121e4[];
extern u32 data_ov010_02112234[];
extern u32 data_ov010_02112358[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_0211222c[2] = {
    0x00000001,
    (u32)data_ov010_02112224,
};

u32 data_ov010_02112224[2] = {
    0x00000100,
    (u32)data_ov010_02112258,
};

u32 data_ov010_0211221c[2] = {
    0x00000001,
    (u32)data_ov010_021121f4,
};

u32 data_ov010_02112214[2] = {
    0x00000500,
    (u32)data_ov010_02112420,
};

u32 data_ov010_0211220c[2] = {
    0x00000002,
    (u32)data_ov010_02112278,
};

u32 data_ov010_02112204[2] = {
    0x00000001,
    (u32)data_ov010_021121e4,
};

u32 data_ov010_021121fc[2] = {
    0x00000001,
    (u32)data_ov010_02112234,
};

u32 data_ov010_02112234[2] = {
    0x00000400,
    (u32)data_ov010_02112358,
};

#pragma section s end
