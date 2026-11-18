/* ov043 cartridge data at 0x02111bc0, 20 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111bc0 ".data.gap2" ".data.gap2" ".data.gap2" far_data
#pragma section data_ov043_02111bc0 begin
u32 data_ov043_02111bc0[5] = {
    0x53504c43, 0x00010008, 0x00000fc0, 0x000000ff, 0xffffffff,
};
#pragma section data_ov043_02111bc0 end
