/* ov050 data_ov050_02111720 at 0x02111720 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111720[8] = {
    0xf5740017, 0x0b540190, 0x00000000, 0xffff0000, 0xf5740017, 0x0a8c0190,
    0x00000000, 0xffff0000,
};

#pragma section dc8_section end
