/* Word table, 12 entries. */
#include "types.h"

extern u32 data_ov001_020ad49c[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 data_ov001_020ad4c4[12] = {
    (u32)&data_ov001_020ad49c,
    0xffffffff,
    0xffffffff,
    0x00001000,
    0x00001000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x001f0080,
    0x00008000,
    0x00000000,
};
#pragma section obj end
