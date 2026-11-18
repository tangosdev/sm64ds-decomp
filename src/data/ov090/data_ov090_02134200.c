/* ov090 data_ov090_02134200 at 0x02134200 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov090_02134200[3] = {
    0x00000000, 0xfffc4000, 0x00050000,
};

#pragma section dc8_section end
