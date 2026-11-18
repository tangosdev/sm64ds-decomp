/* ov023 data_ov023_02111b54 at 0x02111b54 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov023_02111b54[3] = {
    0xff0e0000, 0x000e7fff, 0x7fffff0b,
};

#pragma section dc8_section end
