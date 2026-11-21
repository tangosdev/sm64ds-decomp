#include "types.h"

/* Same-sized objects are reversed except the last, which is the order mwcc emits. */

extern u32 data_ov020_02113f10[];
extern u32 data_ov020_02113b68[];
extern u32 data_ov020_02114064[];
extern u32 data_ov020_021138e4[];
extern u32 data_ov020_021143d8[];
extern u32 data_ov020_02113b18[];

u32 data_ov020_0211387c[4] = {
    0x00000500,
    (u32)data_ov020_02113f10,
    0x00000605,
    (u32)data_ov020_02113b68,
};

u32 data_ov020_0211386c[4] = {
    0x00000700,
    (u32)data_ov020_02114064,
    0x00000305,
    (u32)data_ov020_021138e4,
};

u32 data_ov020_0211385c[4] = {
    0xfe708226,
    0x003201f4,
    0xfa888226,
    0x03b601f4,
};

u32 data_ov020_0211384c[4] = {
    0xff9c0043,
    0xfe0c0032,
    0x00000000,
    0x01000000,
};

u32 data_ov020_0211383c[4] = {
    0x00c88226,
    0xfa880320,
    0xfe0ce5ff,
    0xfc7c0258,
};

u32 data_ov020_0211388c[4] = {
    0x00001300,
    (u32)data_ov020_021143d8,
    0x00000505,
    (u32)data_ov020_02113b18,
};
