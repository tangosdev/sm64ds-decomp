/* ov057 data_ov057_02111640 at 0x02111640 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov057_02111640[8] = {
    0x000002ee, 0x40000000, 0xffff0000, 0x055d0002, 0xfaa302ee, 0x60000000,
    0xffff0000, 0x00000002,
};

#pragma section dc8_section end
