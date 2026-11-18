/* ov050 data_ov050_021115bc at 0x021115bc (data, 36 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021115bc[9] = {
    0xffff0000, 0x0000000f, 0xf8f80000, 0xc0000000, 0xffff0000, 0x03e8000f,
    0xfbe60190, 0x80000000, 0xffff0000,
};

#pragma section dc8_section end
