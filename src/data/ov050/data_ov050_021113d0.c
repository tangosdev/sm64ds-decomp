/* ov050 data_ov050_021113d0 at 0x021113d0 (data, 40 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_021113d0[10] = {
    0xe0000000, 0x00040000, 0xfe7000fe, 0x07080000, 0x80000000, 0x01040000,
    0x070800fe, 0xfe700000, 0x00000000, 0x02040000,
};

#pragma section dc8_section end
