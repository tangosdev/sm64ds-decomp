#include "types.h"

extern u32 data_ov020_0211384c[];
extern u32 data_ov020_0211389c[];
extern u32 data_ov020_0211380c[];
extern u32 data_ov020_0211381c[];

u32 data_ov020_02113ccc[2] = {
    (u32)data_ov020_0211384c,
    0x00000180,
};

u32 data_ov020_02113cd4[6] = {
    (u32)data_ov020_0211389c, 0x000001a0, (u32)data_ov020_0211380c, 0x000001c0,
    (u32)data_ov020_0211381c, 0x000001e0,
};
