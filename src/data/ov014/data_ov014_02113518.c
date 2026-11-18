/* ov014 cartridge data at 0x02113518, 28 bytes. */
#include "types.h"

extern u32 data_ov014_02113380[];
extern u32 data_ov014_02113490[];
extern u32 data_ov014_021134c8[];

#pragma define_section data_ov014_02113518 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113518 begin
u32 data_ov014_02113518[7] = {
    0x00000404, (u32)&data_ov014_02113490, 0x0000010b, (u32)&data_ov014_02113380, 0x00000401, (u32)&data_ov014_021134c8,
    0x00000406,
};
#pragma section data_ov014_02113518 end
