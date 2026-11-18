/* ov014 cartridge data at 0x02113c28, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113c28 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113c28 begin
u32 data_ov014_02113c28[8] = {
    0x16da013b, 0xf254076c, 0x00020006, 0x0023ffff, 0x1806013c, 0xee080bb8,
    0x40000000, 0xffc003e3,
};
#pragma section data_ov014_02113c28 end
