/* ov050 data_ov050_02111218 at 0x02111218 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111218[2] = {
    0x03200001, 0x0000001f,
};

#pragma section dc8_section end
