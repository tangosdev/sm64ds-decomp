/* ov040 data_ov040_021111d4 at 0x021111d4 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_021111d4[2] = {
    0x00000002,
    0x02111200,  /* data_ov040_02111200 */
};

#pragma section dc8_section end
