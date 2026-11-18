/* ov017 data_ov017_02111b44 at 0x02111b44 (data, .data.ksRest, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data.ksRest" ".data.ksRest" ".data.ksRest" far_data
#pragma section dc8_section begin
u32 data_ov017_02111b44[8] = {
    0x80000000, 0x00800000, 0x047e0000, 0xfa880374, 0x80000000, 0x00800000,
    0x05460000, 0xfa880374,
};

#pragma section dc8_section end
