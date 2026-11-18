/* ov043 cartridge data at 0x02111c00, 32 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111c00 ".data.gap2" ".data.gap2" ".data.gap2" far_data
#pragma section data_ov043_02111c00 begin
u32 data_ov043_02111c00[8] = {
    0x53504c43, 0x00020008, 0x00000fc0, 0x000000ff, 0x00002fc0, 0x000000ff,
    0xffffffff, 0xffffffff,
};
#pragma section data_ov043_02111c00 end
