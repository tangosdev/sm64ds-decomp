/* ov040 data_ov040_02111310 at 0x02111310 (data, 96 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_02111310[24] = {
    0x53504c43, 0x00080008, 0x01048fc0, 0x000000ff, 0x0004afc4, 0x000000ff,
    0x01033fc4, 0x000000ff, 0x00048fc4, 0x000000ff, 0x00030fc0, 0x000000ff,
    0x00048044, 0x000000ff, 0x0004a044, 0x000000ff, 0x018c804c, 0x000000ff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};

#pragma section dc8_section end
