/* Word table, 2 entries. */
#include "types.h"

extern u32 data_ov001_020ab620[];

#pragma define_section obj ".rodata" ".rodata" ".rodata" far_data
#pragma section obj begin
extern const u32 data_ov001_020ab618[2] = {
    0x00000001,
    (u32)&data_ov001_020ab620,
};
#pragma section obj end
