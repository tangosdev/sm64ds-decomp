/* ov029 cartridge data at 0x02112eb8, 8 bytes. */
#include "types.h"

extern u32 data_ov029_02113174[];

#pragma define_section data_ov029_02112eb8 ".data.w12ea0" ".data.w12ea0" ".data.w12ea0" far_data
#pragma section data_ov029_02112eb8 begin
u32 data_ov029_02112eb8[2] = {
    0x00000008, (u32)&data_ov029_02113174,
};
#pragma section data_ov029_02112eb8 end
