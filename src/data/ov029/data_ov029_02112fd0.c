/* ov029 cartridge data at 0x02112fd0, 28 bytes. */
#include "types.h"

extern u32 data_ov029_02112eb8[];
extern u32 data_ov029_02112fa0[];
extern u32 data_ov029_02113204[];

#pragma define_section data_ov029_02112fd0 ".data.w12ea0" ".data.w12ea0" ".data.w12ea0" far_data
#pragma section data_ov029_02112fd0 begin
u32 data_ov029_02112fd0[7] = {
    (u32)&data_ov029_02113204, (u32)&data_ov029_02112eb8, 0x07ed07f0, 0x07f207f1, (u32)&data_ov029_02112fa0, 0x03e80002,
    0x0000008f,
};
#pragma section data_ov029_02112fd0 end
