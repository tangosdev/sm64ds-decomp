/* ov050 data_ov050_021111e0 at 0x021111e0 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021111e0[3] = {
    0xff020000, 0x00027fff, 0x7fffff02,
};

#pragma section dc8_section end
