/* Word table, 17 entries. */
#include "types.h"

extern u32 data_ov001_020ab608;
extern u32 data_ov001_020ab60c;
extern u32 data_ov001_020ad4b4[];

#pragma define_section obj ".data" ".data" ".data" far_data
#pragma section obj begin
u32 data_ov001_020ad5dc[17] = {
    0x00000000,
    (u32)&data_ov001_020ad4b4,
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
    (u32)&data_ov001_020ab608,
    (u32)&data_ov001_020ab60c,
    0x00000000,
    0x00000000,
};
#pragma section obj end
