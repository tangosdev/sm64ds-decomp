/* ov042 data_ov042_02111264 at 0x02111264 (data, 64 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov042_02111264[16] = {
    0x53504c43, 0x00060008, 0x01040fc0, 0x000000ff, 0x001c0fc0, 0x000000ff,
    0x01043fc0, 0x000000ff, 0x00042fc0, 0x000000ff, 0x00740fc4, 0x000000ff,
    0x01740fc4, 0x000000ff, 0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
