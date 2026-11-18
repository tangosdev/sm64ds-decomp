/* ov049 data_ov049_02111204 at 0x02111204 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov049_02111204[3] = {
    0x07da07db,
    0x00000000,
    0x021111d8,  /* data_ov049_021111d8 */
};

#pragma section dc8_section end
