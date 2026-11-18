/* ov054 data_ov054_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111520[7] = {
    0x0258073a, 0x0000fffc, 0x00004000, 0x1b56ff02, 0xeb0c0258, 0x80000000,
    0x01040000,
};

#pragma section dc8_section end
