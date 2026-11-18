/* ov050 data_ov050_02111220 at 0x02111220 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111220[2] = {
    0x00000402,
    0x021111ec,  /* data_ov050_021111ec */
};

#pragma section dc8_section end
