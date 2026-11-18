/* ov031 data_ov031_02111680 at 0x02111680 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111680[8] = {
    0xa0000000, 0x00f20000, 0xcfda0031, 0xf6960ad9, 0x80000000, 0x00f20000,
    0x109f00c0, 0x07f723b7,
};

#pragma section dc8_section end
