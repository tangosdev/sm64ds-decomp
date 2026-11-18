/* ov016 cartridge data at 0x02113b28, 4 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113b28 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b28 begin
u32 data_ov016_02113b28[1] = {
    0xed0e15ae,
};
#pragma section data_ov016_02113b28 end
