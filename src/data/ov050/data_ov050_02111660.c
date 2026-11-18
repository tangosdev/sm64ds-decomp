/* ov050 data_ov050_02111660 at 0x02111660 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111660[8] = {
    0xf95c013b, 0xf95c0514, 0x00040001, 0x0020ffff, 0xfaec013c, 0xfaec06a4,
    0xa0000000, 0xffc003e0,
};

#pragma section dc8_section end
