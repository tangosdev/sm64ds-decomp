/* ov011 data_ov011_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov011_02111520[7] = {
    0x80000000, 0x00000000, 0x00000000, 0x012c0000, 0x80000000, 0x07000000,
    0x0cb20000,
};

#pragma section dc8_section end
