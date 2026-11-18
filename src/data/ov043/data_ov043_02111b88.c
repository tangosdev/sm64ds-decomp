/* ov043 cartridge data at 0x02111b88, 24 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111b88 ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section data_ov043_02111b88 begin
u32 data_ov043_02111b88[6] = {
    0x00000fc0, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};
#pragma section data_ov043_02111b88 end
