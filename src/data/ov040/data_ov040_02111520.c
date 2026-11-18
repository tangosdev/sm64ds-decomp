/* ov040 data_ov040_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_02111520[7] = {
    0x05910008, 0xe822f92a, 0x40000000, 0x00020000, 0x05910008, 0xe822f92a,
    0x40000000,
};

#pragma section dc8_section end
