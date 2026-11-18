/* ov048 data_ov048_02111780 at 0x02111780 (data, 64 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov048_02111780[16] = {
    0x53504c43, 0x00040008, 0x00038fc4, 0x000000ff, 0x00038fc0, 0x000000ff,
    0x00238fc0, 0x000000ff, 0x01038fc0, 0x000000ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
