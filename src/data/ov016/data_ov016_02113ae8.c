/* ov016 cartridge data at 0x02113ae8, 8 bytes. */
#include "types.h"

extern u32 data_ov016_02113d8c[];

#pragma define_section data_ov016_02113ae8 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113ae8 begin
u32 data_ov016_02113ae8[2] = {
    0x00000009,
    (u32)&data_ov016_02113d8c,
};
#pragma section data_ov016_02113ae8 end
