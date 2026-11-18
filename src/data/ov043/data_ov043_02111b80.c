/* ov043 cartridge data at 0x02111b80, 8 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111b80 ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section data_ov043_02111b80 begin
u32 data_ov043_02111b80[2] = {
    0x53504c43, 0x00010008,
};
#pragma section data_ov043_02111b80 end
