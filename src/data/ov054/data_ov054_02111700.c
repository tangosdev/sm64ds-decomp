/* ov054 data_ov054_02111700 at 0x02111700 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111700[8] = {
    0x000000ff, 0x00040fc5, 0x000000ff, 0x000400c5, 0x000000ff, 0x00030144,
    0x000000ff, 0x00032fc4,
};

#pragma section dc8_section end
