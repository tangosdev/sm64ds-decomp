/* ov050 data_ov050_02111740 at 0x02111740 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111740[8] = {
    0x00640017, 0xfbe60000, 0x00000000, 0xffff0000, 0xff9c0017, 0xfbe60000,
    0x00000000, 0xffff0000,
};

#pragma section dc8_section end
