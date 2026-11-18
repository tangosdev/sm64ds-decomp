/* ov042 data_ov042_021111d4 at 0x021111d4 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov042_021111d4[2] = {
    0x00000006,
    0x02111234,  /* data_ov042_02111234 */
};

#pragma section dc8_section end
