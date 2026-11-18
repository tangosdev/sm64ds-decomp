/* ov051 data_ov051_02111550 at 0x02111550 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov051_02111550[3] = {
    0x00000402,
    0x021114f4,  /* data_ov051_021114f4 */
    0x00000103,
};

#pragma section dc8_section end
