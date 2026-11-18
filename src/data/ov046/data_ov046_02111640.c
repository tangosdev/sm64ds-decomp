/* ov046 data_ov046_02111640 at 0x02111640 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov046_02111640[8] = {
    0x0dac04b0, 0x00000000, 0xffff0000, 0x0dac00ec, 0x000004b0, 0x00000000,
    0xffff0000, 0x000000ec,
};

#pragma section dc8_section end
