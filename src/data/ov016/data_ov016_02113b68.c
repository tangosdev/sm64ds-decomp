/* ov016 cartridge data at 0x02113b68, 16 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b68 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b68 begin
u32 data_ov016_02113b68[4] = {
    0xba875701,
    0xb3b02af8,
    0xc0700001,
    0x94700000,
};
#pragma section data_ov016_02113b68 end
