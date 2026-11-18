/* ov043 cartridge data at 0x02111b18, 12 bytes. */
#include "types.h"

extern u32 data_ov043_02111b10[];

#pragma define_section data_ov043_02111b18 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov043_02111b18 begin
u32 data_ov043_02111b18[3] = {
    (u32)&data_ov043_02111b10, 0x00000000, 0x0af00000,
};
#pragma section data_ov043_02111b18 end
