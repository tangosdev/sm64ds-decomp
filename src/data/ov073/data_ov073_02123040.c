/* ov073 data_ov073_02123040 at 0x02123040 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov073_02123040[3] = {
    0x00000000, 0xfffa0000, 0x00000000,
};

#pragma section dc8_section end
