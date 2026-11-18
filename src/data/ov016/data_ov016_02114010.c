/* ov016 cartridge data at 0x02114010, 168 bytes. */
#include "types.h"

#pragma define_section data_ov016_02114010 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02114010 begin
u32 data_ov016_02114010[42] = {
    0x12e6e225, 0x0258eed4, 0x1476e225, 0x0258eed4, 0x0032e225, 0x09380384, 0x0032e225,
    0x0b900384, 0xff06e225, 0x0a640384, 0x015ee225, 0x0a640384, 0x00fae225, 0x099c0384,
    0xff6ae225, 0x099c0384, 0xff6ae225, 0x0b2c0384, 0x00fae225, 0x0b2c0384, 0x02eeae26,
    0x1036fe20, 0xf808ae26, 0x0e38fc2c, 0xfe16ae26, 0x16b20208, 0xf7fee225, 0xee3af4af,
    0xf7fee225, 0xf15af4af, 0xf66ee225, 0xefcaf4af, 0xf98ee225, 0xefcaf4af, 0xf92ae225,
    0xee9ef4af, 0xf6d2e225, 0xee9ef4af, 0xf6d2e225, 0xf0f6f4af, 0xf92ae225, 0xf0f6f4af,
};
#pragma section data_ov016_02114010 end
