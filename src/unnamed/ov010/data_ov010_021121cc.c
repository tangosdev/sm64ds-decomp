#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_02112398[];
extern u32 data_ov010_021121d4[];
extern u32 data_ov010_021122b8[];
extern u32 data_ov010_02112268[];
extern u32 data_ov010_021123d8[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_021121e4[2] = {
    0x00000400,
    (u32)data_ov010_02112398,
};

u32 data_ov010_021121dc[2] = {
    0x00000001,
    (u32)data_ov010_021121d4,
};

u32 data_ov010_021121d4[2] = {
    0x00000200,
    (u32)data_ov010_021122b8,
};

u32 data_ov010_021121cc[2] = {
    0x00000002,
    (u32)data_ov010_02112268,
};

u32 data_ov010_021121ec[2] = {
    0x00000009,
    (u32)data_ov010_021123d8,
};

#pragma section s end
