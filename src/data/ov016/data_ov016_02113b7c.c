/* ov016 cartridge data at 0x02113b7c, 4 bytes. */
#include "types.h"

extern u32 data_ov016_02113550[];

#pragma define_section data_ov016_02113b7c ".data.slideBox" ".data.slideBox" ".data.slideBox" far_data
#pragma section data_ov016_02113b7c begin
u32 data_ov016_02113b7c[1] = {
    (u32)&data_ov016_02113550,
};
#pragma section data_ov016_02113b7c end
