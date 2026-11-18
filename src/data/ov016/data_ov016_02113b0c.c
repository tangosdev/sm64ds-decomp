/* ov016 cartridge data at 0x02113b0c, 4 bytes. */
#include "types.h"

extern u32 data_ov016_02113ae8[];

#pragma define_section data_ov016_02113b0c ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b0c begin
u32 data_ov016_02113b0c[1] = {
    (u32)&data_ov016_02113ae8,
};
#pragma section data_ov016_02113b0c end
