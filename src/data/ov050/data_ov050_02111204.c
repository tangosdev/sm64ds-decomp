/* ov050 data_ov050_02111204 at 0x02111204 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111204[3] = {
    0x02111318,  /* data_ov050_02111318 */
    0x021111cc,  /* data_ov050_021111cc */
    0x94009402,
};

#pragma section dc8_section end
