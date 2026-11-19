/* ov029 .data with no class source: the CLPS block at 0x021130ac. */
#include "types.h"

struct CLPS_Block {
    u32 words[8];
};

struct CLPS_Block data_ov029_021130ac = {
    0x53504c43, 0x00020008, 0x01000fc4, 0x000000ff, 0x01000fc0, 0x000000ff, 0xffffffff, 0xffffffff,
};
