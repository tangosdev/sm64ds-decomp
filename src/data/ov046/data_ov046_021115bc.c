/* ov046 data_ov046_021115bc at 0x021115bc (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov046_021115bc[8] = {
    0x53504c43, 0x00020008, 0x00000fc0, 0x000000ff, 0x00038fc4, 0x000000ff,
    0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
