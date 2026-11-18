/* Word table, 4 entries. */
#include "types.h"

extern u32 data_ov001_020ab604;
extern u32 data_ov001_020ab640[];

#pragma define_section obj ".rodata" ".rodata" ".rodata" far_data
#pragma section obj begin
extern const u32 data_ov001_020ab620[4] = {
    0x00000001,
    (u32)&data_ov001_020ab604,
    0x00000070,
    (u32)&data_ov001_020ab640,
};
#pragma section obj end
