/* ov016 cartridge data at 0x02114380, 124 bytes. */
#include "types.h"

#pragma define_section data_ov016_02114380 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02114380 begin
u32 data_ov016_02114380[31] = {
    0xcaaa0000, 0xffff0000, 0xf89900b8, 0x09110190, 0x80000000, 0xffff0000, 0xf92400b8, 0x1084fd44,
    0x00000000, 0xffff0000, 0xfcc600b8, 0x1d3f0384, 0xd5550000, 0xffff0000, 0xfce300b8, 0x0d90fda8,
    0xe0000000, 0xffff0000, 0x00f900b8, 0x1200ff9c, 0x20000000, 0xffff0000, 0x06d100b8, 0x1394fed4,
    0xe0000000, 0xffff0000, 0x041d00b8, 0x08c5fda8, 0x3c710000, 0xffff0000, 0x0ae000b8,
};
#pragma section data_ov016_02114380 end
