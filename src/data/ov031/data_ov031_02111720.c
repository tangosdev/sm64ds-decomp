/* ov031 data_ov031_02111720 at 0x02111720 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111720[8] = {
    0x00000000, 0xffff0000, 0x229b00c3, 0x19f82dc0, 0x00000000, 0xffff0000,
    0xdabc00c3, 0x13341cc4,
};

#pragma section dc8_section end
