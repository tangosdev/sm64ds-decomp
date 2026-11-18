/* ov038 data_ov038_021111dc at 0x021111dc (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_021111dc[3] = {
    0x021111cc,  /* data_ov038_021111cc */
    0x00000000,
    0x000a0000,
};

#pragma section dc8_section end
