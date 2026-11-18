/* ov050 data_ov050_021111c4 at 0x021111c4 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021111c4[2] = {
    0x00000005,
    0x02111248,  /* data_ov050_02111248 */
};

#pragma section dc8_section end
