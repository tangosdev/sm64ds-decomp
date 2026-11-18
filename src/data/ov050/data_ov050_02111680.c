/* ov050 data_ov050_02111680 at 0x02111680 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111680[8] = {
    0xf4ac0017, 0x0b540190, 0x00003e93, 0xffff0000, 0x01f4013b, 0xf95c0000,
    0x00040001, 0x0021ffff,
};

#pragma section dc8_section end
