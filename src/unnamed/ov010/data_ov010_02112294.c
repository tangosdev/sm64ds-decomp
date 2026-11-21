#include "types.h"

/* A class section sits between this run and .data, so the objects share one named section. */

extern u32 data_ov010_021126d0[];
extern u32 data_ov010_021121ec[];
extern u32 data_ov010_021124c0[];

#pragma define_section s ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" ".data.data_ov010_021121c0" far_data
#pragma section s begin
u32 data_ov010_02112294[2] = {
    0xff0c001a,
    0x0000ffff,
};

u32 data_ov010_0211229c[7] = {
    (u32)data_ov010_021126d0,
    (u32)data_ov010_021121ec,
    0x06e806f0,
    0x06f206f1,
    (u32)data_ov010_021124c0,
    0x06400008,
    0x0000000f,
};

u32 data_ov010_021122b8[8] = {
    0xfb820043,
    0xf8940000,
    0x00000000,
    0xff040000,
    0xfb820043,
    0xf8940000,
    0x00000000,
    0xff0b0000,
};

#pragma section s end
