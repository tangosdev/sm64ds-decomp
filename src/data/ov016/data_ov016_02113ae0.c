/* ov016 cartridge data at 0x02113ae0, 4 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113ae0 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113ae0 begin
u32 data_ov016_02113ae0[1] = {
    0x00000760,
};
#pragma section data_ov016_02113ae0 end
