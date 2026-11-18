/* ov051 data_ov051_02111700 at 0x02111700 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov051_02111700[8] = {
    0x0e7400fe, 0x012c005e, 0xc0b70000, 0x02040000, 0x050f0020, 0xf25b0226,
    0x00000000, 0xffff0000,
};

#pragma section dc8_section end
