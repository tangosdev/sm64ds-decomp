/* ov054 data_ov054_02111208 at 0x02111208 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111208[2] = {
    0x00000600,
    0x021114b0,  /* data_ov054_021114b0 */
};

#pragma section dc8_section end
