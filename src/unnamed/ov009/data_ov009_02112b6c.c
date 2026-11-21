#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov009_02112dcc[];
extern u32 data_ov009_02112c9c[];

#pragma define_section s ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" ".data.data_ov009_02112b60" far_data
#pragma section s begin
u32 data_ov009_02112b74[2] = {
    0x00000009,
    (u32)&data_ov009_02112dcc,
};

u32 data_ov009_02112b6c[2] = {
    0x00000006,
    (u32)&data_ov009_02112c9c,
};

u32 data_ov009_02112b7c[2] = {
    0x65746177,
    0x616d5f72,
};

#pragma section s end
