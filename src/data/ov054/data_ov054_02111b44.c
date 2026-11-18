/* ov054 data_ov054_02111b44 at 0x02111b44 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111b44[8] = {
    0xffff0000, 0x1b5800d2, 0xe4760000, 0x00000000, 0xffff0000, 0x1b5800d2,
    0xe4120000, 0x00000000,
};

#pragma section dc8_section end
