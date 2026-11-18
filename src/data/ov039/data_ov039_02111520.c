/* ov039 data_ov039_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov039_02111520[7] = {
    0x00000000, 0xff050000, 0x0000001b, 0x0000f830, 0x00000000, 0xffff0000,
    0xea840106,
};

#pragma section dc8_section end
