/* ov050 data_ov050_02111210 at 0x02111210 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111210[2] = {
    0x94049403,
    0x021111d4,  /* data_ov050_021111d4 */
};

#pragma section dc8_section end
