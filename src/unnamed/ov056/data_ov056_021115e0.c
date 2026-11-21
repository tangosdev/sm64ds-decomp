#include "types.h"

extern u32 data_ov056_02112bec[];
extern u32 data_ov056_02112c24[];

const u32 data_ov056_021115e0[1] = {
    0x00000001,
};

const u32 data_ov056_021115e4[4] = {
    0x0000021d,
    0x0000021d,
    0x0000ffff,
    (u32)&data_ov056_02112bec,
};

const u32 data_ov056_021115f4[12] = {
    0x00000001,
    0x00000001,
    0x0001021d,
    0x00000001,
    0x00000001,
    0x0000ffff,
    (u32)&data_ov056_02112c24,
    0x00000001,
    0x00000001,
    0x00000001,
    0x021d021d,
    0x00000001,
};
