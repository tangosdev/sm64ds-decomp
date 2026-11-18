/* ov014 cartridge data at 0x0211382c, 52 bytes. */
#include "types.h"

#pragma define_section data_ov014_0211382c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_0211382c begin
u32 data_ov014_0211382c[13] = {
    0x00010000, 0xef8b0014, 0x13870000, 0xc00000ff, 0x0b260000, 0x10b000fe, 0xe6a40d06,
    0x00000000, 0x20110000, 0xff39000f, 0x107d0000, 0xc0000000, 0xf0f00000,
};
#pragma section data_ov014_0211382c end
