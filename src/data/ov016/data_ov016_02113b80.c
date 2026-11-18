/* ov016 cartridge data at 0x02113b80, 16 bytes. */
#include "types.h"

extern u32 data_ov016_0211354c[];
extern u32 data_ov016_02113554[];
extern u32 data_ov016_02113570[];

#pragma define_section data_ov016_02113b80 ".data.slideBox"                                        \
                                           ".data.slideBox"                                        \
                                           ".data.slideBox" far_data
#pragma section data_ov016_02113b80 begin
u32 data_ov016_02113b80[4] = {
    (u32)&data_ov016_0211354c,
    (u32)&data_ov016_02113570,
    0x00000001,
    (u32)&data_ov016_02113554,
};
#pragma section data_ov016_02113b80 end
