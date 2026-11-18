/* ov038 data_ov038_021111cc at 0x021111cc (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_021111cc[2] = {
    0x00000002,
    0x02111208,  /* data_ov038_02111208 */
};

#pragma section dc8_section end
