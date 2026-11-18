/* ov011 data_ov011_02111700 at 0x02111700 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov011_02111700[8] = {
    0x07d00043, 0xf63cff9c, 0xe0000000, 0xff0a0000, 0x07080043, 0xfd44ff9c,
    0x40000000, 0xff0a0000,
};

#pragma section dc8_section end
