/* ov016 cartridge data at 0x02114824, 60 bytes. */
#include "types.h"

#pragma define_section data_ov016_02114824 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02114824 begin
u32 data_ov016_02114824[15] = {
    0xf0010000, 0xe5a20017, 0x012c044c, 0x238e0000, 0xffff0000, 0x0abe004c, 0x1bbc05dc, 0xaaabffff,
    0x07550000, 0xe6230017, 0x0092044c, 0x238e0000, 0xffff0000, 0x108f0019, 0x127d0514,
};
#pragma section data_ov016_02114824 end
