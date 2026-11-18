/* ov016 cartridge data at 0x02113af0, 8 bytes. */
#include "types.h"

extern u32 data_ov016_02113d4c[];

#pragma define_section data_ov016_02113af0 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113af0 begin
u32 data_ov016_02113af0[2] = {
    0x00000008,
    (u32)&data_ov016_02113d4c,
};
#pragma section data_ov016_02113af0 end
