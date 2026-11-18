/* ov057 data_ov057_0211157c at 0x0211157c (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov057_0211157c[8] = {
    0x53504c43, 0x00020008, 0x00080fc0, 0x000000ff, 0x01000fc0, 0x000000ff,
    0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
