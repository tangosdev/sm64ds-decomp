#include "types.h"

extern const u32 data_ov056_021115d0[];
extern u32 data_ov056_02112bc0[];

u32 data_ov056_02112c44[2] = {
    0x00000003,
    (u32)&data_ov056_021115d0,
};

u32 data_ov056_02112c4c[3] = {
    (u32)&data_ov056_02112bc0[97],
    (u32)&data_ov056_02112bc0[2],
    0x073b073d,
};

u32 data_ov056_02112c58[4] = {
    0x073f073e,
    (u32)&data_ov056_02112bc0[14],
    0x0a280001,
    0x0000004f,
};

u32 data_ov056_02112c68[8] = {
    0xfd310000, 0xd5940960, 0x00000000, 0x05000000, 0xd9d70000, 0x1710f704, 0x51c70000, 0x04100000,
};
