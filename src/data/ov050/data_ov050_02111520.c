/* ov050 data_ov050_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111520[7] = {
    0xf862e225, 0x08fc00c8, 0xf92ae225, 0x08fc00c8, 0xf9f2e225, 0x08fc00c8,
    0x08cae225,
};

#pragma section dc8_section end
