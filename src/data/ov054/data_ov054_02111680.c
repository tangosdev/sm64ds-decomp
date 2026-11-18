/* ov054 data_ov054_02111680 at 0x02111680 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111680[8] = {
    0xf0600000, 0x00000003, 0xffff0000, 0x1ce80043, 0xef980050, 0x00000000,
    0xff090000, 0x19000043,
};

#pragma section dc8_section end
