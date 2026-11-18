/* ov047 data_ov047_02111b94 at 0x02111b94 (data, .data_mid, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data_mid" ".data_mid" ".data_mid" far_data
#pragma section dc8_section begin
u32 data_ov047_02111b94[8] = {
    0x53504c43, 0x00020008, 0x00000fc0, 0x000000ff, 0x00004fc0, 0x000000ff,
    0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
