/* ov016 cartridge data at 0x02113b10, 8 bytes. */
#include "types.h"

extern u32 data_ov016_02113b78[];

#pragma define_section data_ov016_02113b10 ".data.slideBox" ".data.slideBox" ".data.slideBox" far_data
#pragma section data_ov016_02113b10 begin
u32 data_ov016_02113b10[2] = {
    (u32)&data_ov016_02113b78, 0x0fa00000,
};
#pragma section data_ov016_02113b10 end
