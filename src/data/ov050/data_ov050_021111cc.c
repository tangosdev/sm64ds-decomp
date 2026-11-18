/* ov050 data_ov050_021111cc at 0x021111cc (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021111cc[2] = {
    0x00000005,
    0x02111220,  /* data_ov050_02111220 */
};

#pragma section dc8_section end
