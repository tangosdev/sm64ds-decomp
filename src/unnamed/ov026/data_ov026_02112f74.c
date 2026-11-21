#include "types.h"

typedef struct CLPS_Block {
    u32 words[2];
} CLPS_Block;

extern u32 data_ov026_021132b0[];
extern u32 data_ov026_02113600[];
extern u32 data_ov026_02112e90[];
extern u32 data_ov026_02112ed0[];
extern u32 data_ov026_02112eb0[];
extern u32 data_ov026_02112ec0[];
extern u32 data_ov026_02112e60[];

CLPS_Block data_ov026_02112f74 = {
    0x53504c43,
    0x00010008,
};

u32 data_ov026_02112f7c[6] = {
    0x00000fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

u32 data_ov026_02112f94[15] = {
    0x00001a00, (u32)data_ov026_021132b0, 0x00003805, (u32)data_ov026_02113600,
    0x00000140, (u32)data_ov026_02112e90, 0x00000160, (u32)data_ov026_02112ed0,
    0x00000180, (u32)data_ov026_02112eb0, 0x000001a0, (u32)data_ov026_02112ec0,
    0x000001c0, (u32)data_ov026_02112e60, 0x000001e0,
};
