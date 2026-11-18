/* ov040 data_ov040_02111680 at 0x02111680 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_02111680[8] = {
    0xe7c8001e, 0x0dde01c2, 0x00000000, 0xff060000, 0xf7180014, 0xe796f6a0,
    0x15550003, 0xffff0000,
};

#pragma section dc8_section end
