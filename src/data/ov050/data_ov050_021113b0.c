/* ov050 data_ov050_021113b0 at 0x021113b0 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021113b0[8] = {
    0x20000000, 0x11040000, 0x03e800fe, 0xfbe60190, 0x60000000, 0x12040000,
    0xfed400fe, 0xfed40000,
};

#pragma section dc8_section end
