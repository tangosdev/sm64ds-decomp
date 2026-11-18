/* ov016 cartridge data at 0x02113b90, 28 bytes. */
#include "types.h"

extern u32 data_ov016_02113af0[];
extern u32 data_ov016_02113b0c[];
extern u32 data_ov016_02113dd4[];

#pragma define_section data_ov016_02113b90 ".data.slideBox" ".data.slideBox" ".data.slideBox" far_data
#pragma section data_ov016_02113b90 begin
u32 data_ov016_02113b90[7] = {
    (u32)&data_ov016_02113dd4, (u32)&data_ov016_02113af0, 0x075f0761, 0x07630762, (u32)&data_ov016_02113b0c, 0x06400001,
    0x0000003f,
};
#pragma section data_ov016_02113b90 end
