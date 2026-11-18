/* Word table, 16 entries. */
#include "types.h"

extern u32 data_ov001_020ab5f8;
extern u32 data_ov001_020ab5fc;
extern u32 data_ov001_020ad4a8[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 data_ov001_020ad59c[16] = {
    0x00000000,
    (u32)&data_ov001_020ad4a8,
    0x00000000,
    0x00000000,
    0x00001000,
    0x00001000,
    0x00001000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000001,
    (u32)&data_ov001_020ab5fc,
    (u32)&data_ov001_020ab5f8,
    0x00000000,
};
#pragma section obj end
