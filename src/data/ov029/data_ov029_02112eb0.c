/* ov029 cartridge data at 0x02112eb0, 8 bytes. */
#include "types.h"

extern u32 data_ov029_021131b4[];

#pragma define_section data_ov029_02112eb0 ".data.w12ea0" ".data.w12ea0" ".data.w12ea0" far_data
#pragma section data_ov029_02112eb0 begin
u32 data_ov029_02112eb0[2] = {
    0x0000000a, (u32)&data_ov029_021131b4,
};
#pragma section data_ov029_02112eb0 end
