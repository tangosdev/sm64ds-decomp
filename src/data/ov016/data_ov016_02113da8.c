/* ov016 cartridge data at 0x02113da8, 28 bytes. */
#include "types.h"

extern u32 data_ov016_02113b48[];
extern u32 data_ov016_02113c0c[];
extern u32 data_ov016_02113c2c[];
extern u32 data_ov016_02113c8c[];

#pragma define_section data_ov016_02113da8 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02113da8 begin
u32 data_ov016_02113da8[7] = {
    (u32)&data_ov016_02113c8c, 0x00000260, (u32)&data_ov016_02113c0c, 0x00000180,
    (u32)&data_ov016_02113b48, 0x000002a0, (u32)&data_ov016_02113c2c,
};
#pragma section data_ov016_02113da8 end
