/* ov049 data_ov049_02111218 at 0x02111218 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov049_02111218[4] = {
    0x53504c43, 0x00010008, 0x00040fc0, 0x000000ff,
};

#pragma section dc8_section end
