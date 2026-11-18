/* ov038 data_ov038_02111234 at 0x02111234 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_02111234[4] = {
    0x00000031, 0x0000f1f0, 0x00000000, 0x00f80000,
};

#pragma section dc8_section end
