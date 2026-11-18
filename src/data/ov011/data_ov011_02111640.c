/* ov011 data_ov011_02111640 at 0x02111640 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov011_02111640[8] = {
    0x0bcc004c, 0x00dc0014, 0xa000ffff, 0x07440000, 0xf434004c, 0x00dc0014,
    0x6000ffff, 0x07450000,
};

#pragma section dc8_section end
