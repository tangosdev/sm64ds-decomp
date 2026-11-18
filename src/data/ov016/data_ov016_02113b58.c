/* ov016 cartridge data at 0x02113b58, 16 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b58 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b58 begin
u32 data_ov016_02113b58[4] = {
    0x0ed800fe,
    0x0672ec78,
    0x15550000,
    0x10040000,
};
#pragma section data_ov016_02113b58 end
