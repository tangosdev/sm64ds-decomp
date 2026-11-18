/* ov028 data_ov028_02111414 at 0x02111414 (data, 36 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov028_02111414[9] = {
    0x53504c43, 0x00050008, 0x018b0fcc, 0x000000ff, 0x00030fc5, 0x000000ff,
    0x00033fc6, 0x000000ff, 0x00010fe0,
};

#pragma section dc8_section end
