/* ov051 data_ov051_02111520 at 0x02111520 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov051_02111520[2] = {
    0x06400001, 0x0000001f,
};

#pragma section dc8_section end
