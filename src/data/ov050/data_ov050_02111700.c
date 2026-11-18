/* ov050 data_ov050_02111700 at 0x02111700 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111700[8] = {
    0xfbe60017, 0xff9c0000, 0x00000000, 0xffff0000, 0xfbe60017, 0xfed40000,
    0x00000000, 0xffff0000,
};

#pragma section dc8_section end
