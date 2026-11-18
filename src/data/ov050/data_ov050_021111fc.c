/* ov050 data_ov050_021111fc at 0x021111fc (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021111fc[2] = {
    0x03e80b22, 0xf6a00000,
};

#pragma section dc8_section end
