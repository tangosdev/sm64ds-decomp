#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov036_02113384[];
extern u32 data_ov036_0211306c[];
extern u32 data_ov036_02112a44[];
extern u32 data_ov036_02112d1c[];
extern u32 data_ov036_02112a10[];
extern u32 data_ov036_02112a18[];

#pragma define_section s ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" ".data.data_ov036_02112a00" far_data
#pragma section s begin
u32 data_ov036_02112a54[6] = {
    0x00006d00,
    (u32)&data_ov036_02113384,
    0x00006305,
    (u32)&data_ov036_0211306c,
    0x00000160,
    (u32)&data_ov036_02112a44,
};

u32 data_ov036_02112a6c[7] = {
    (u32)&data_ov036_02112d1c,
    (u32)&data_ov036_02112a10,
    0x07a907ab,
    0x07ad07ac,
    (u32)&data_ov036_02112a18,
    0x06400101,
    0x0000002f,
};

#pragma section s end
