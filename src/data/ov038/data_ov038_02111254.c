/* ov038 data_ov038_02111254 at 0x02111254 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_02111254[8] = {
    0x53504c43, 0x00020008, 0x00040fc0, 0x000000ff, 0x00040fe0, 0x000000ff,
    0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
