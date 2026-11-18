/* ov016 cartridge data at 0x02113b48, 8 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b48 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b48 begin
u32 data_ov016_02113b48[2] = {
    0x0ed800fe,
    0x0672ec78,
};
#pragma section data_ov016_02113b48 end
