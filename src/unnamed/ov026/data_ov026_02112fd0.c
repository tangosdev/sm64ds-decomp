#include "types.h"

typedef struct CLPS_Block {
    u32 words[10];
} CLPS_Block;

extern u32 data_ov026_02112e70[];

u32 data_ov026_02112fd0[1] = {
    (u32)data_ov026_02112e70,
};

CLPS_Block data_ov026_02112fd4 = {
    0x53504c43, 0x00040008, 0x00000fc5, 0x000000ff, 0x00004fc4,
    0x000000ff, 0x01000fc4, 0x000000ff, 0x01000fc5, 0x000000ff,
};
