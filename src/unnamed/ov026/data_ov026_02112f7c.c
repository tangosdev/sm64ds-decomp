/* Overlay 26 second .data run: plain words. */
#include "types.h"

/* Words the overlay relocates point here. */
extern u32 data_ov026_02112e60[];
extern u32 data_ov026_02112e70[];
extern u32 data_ov026_02112e90[];
extern u32 data_ov026_02112eb0[];
extern u32 data_ov026_02112ec0[];
extern u32 data_ov026_02112ed0[];
extern u32 data_ov026_021132b0[];
extern u32 data_ov026_02113600[];

/* 0x02112f7c..0x02112fd4 (88 bytes, 22 words) */
u32 data_ov026_02112f7c[] = {
    0x00000fc0, 0x000000ff,
    0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff,
    0x00001a00, (u32)data_ov026_021132b0,
    0x00003805, (u32)data_ov026_02113600,
    0x00000140, (u32)data_ov026_02112e90,
    0x00000160, (u32)data_ov026_02112ed0,
    0x00000180, (u32)data_ov026_02112eb0,
    0x000001a0, (u32)data_ov026_02112ec0,
    0x000001c0, (u32)data_ov026_02112e60,
    0x000001e0, (u32)data_ov026_02112e70,
};
