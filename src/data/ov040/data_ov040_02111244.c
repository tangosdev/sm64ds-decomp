/* ov040 data_ov040_02111244 at 0x02111244 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_02111244[8] = {
    0x53504c43, 0x00010008, 0x00000fc0, 0x000000ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
