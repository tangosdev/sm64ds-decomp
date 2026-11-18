/* ov038 data_ov038_02111210 at 0x02111210 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_02111210[2] = {
    0x00004105,
    0x021112e4,  /* data_ov038_021112e4 */
};

#pragma section dc8_section end
