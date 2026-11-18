/* ov023 data_ov023_02111c38 at 0x02111c38 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov023_02111c38[7] = {
    0x53504c43, 0x00080008, 0x00030000, 0x000000ff, 0x000b0000, 0x000000ff,
    0x00033000,
};

#pragma section dc8_section end
