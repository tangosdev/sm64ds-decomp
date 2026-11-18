/* ov040 data_ov040_021111dc at 0x021111dc (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov040_021111dc[2] = {
    0x00000008,
    0x021112d0,  /* data_ov040_021112d0 */
};

#pragma section dc8_section end
