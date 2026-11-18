/* ov016 cartridge data at 0x02113b00, 12 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b00 ".data.slideBox" ".data.slideBox" ".data.slideBox" far_data
#pragma section data_ov016_02113b00 begin
u32 data_ov016_02113b00[3] = {
    0xff090000, 0x0009ffff, 0xffffff0d,
};
#pragma section data_ov016_02113b00 end
