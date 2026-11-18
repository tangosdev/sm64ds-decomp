/* ov016 cartridge data at 0x02113b18, 16 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b18 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b18 begin
u32 data_ov016_02113b18[4] = {
    0xe638ff02,
    0xfd4404b0,
    0x20000000,
    0x00000000,
};
#pragma section data_ov016_02113b18 end
