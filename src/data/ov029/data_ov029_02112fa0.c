/* ov029 cartridge data at 0x02112fa0, 24 bytes. */
#include "types.h"

extern u32 data_ov029_02112ea8[];
extern u32 data_ov029_02112eb0[];

#pragma define_section data_ov029_02112fa0 ".data.w12ea0" ".data.w12ea0" ".data.w12ea0" far_data
#pragma section data_ov029_02112fa0 begin
u32 data_ov029_02112fa0[6] = {
    (u32)&data_ov029_02112eb0, 0x00000000, 0x109a0001, (u32)&data_ov029_02112ea8, 0x00000000, 0x00000001,
};
#pragma section data_ov029_02112fa0 end
