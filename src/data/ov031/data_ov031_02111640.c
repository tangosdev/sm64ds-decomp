/* ov031 data_ov031_02111640 at 0x02111640 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111640[8] = {
    0x01008fc0, 0x000000ff, 0x0000dfc0, 0x000000ff, 0x0000dfc5, 0x000000ff,
    0x0000dfc4, 0x000000ff,
};

#pragma section dc8_section end
