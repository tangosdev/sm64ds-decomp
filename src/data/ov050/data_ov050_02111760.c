/* ov050 data_ov050_02111760 at 0x02111760 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111760[2] = {
    0xfed40017, 0xfbe60000,
};

#pragma section dc8_section end
