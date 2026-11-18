/* ov031 data_ov031_02111700 at 0x02111700 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111700[8] = {
    0x00000000, 0xffff0000, 0xe71c00c2, 0x20c9f039, 0x00000000, 0xffff0000,
    0x297900c2, 0xc2a1ddbe,
};

#pragma section dc8_section end
