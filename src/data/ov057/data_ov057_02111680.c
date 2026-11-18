/* ov057 data_ov057_02111680 at 0x02111680 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov057_02111680[8] = {
    0x000002ee, 0xc0000000, 0xffff0000, 0xfaa30002, 0x055d02ee, 0xe0000000,
    0xffff0000, 0x07040003,
};

#pragma section dc8_section end
