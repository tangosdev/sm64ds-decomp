/* ov031 data_ov031_02111520 at 0x02111520 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111520[3] = {
    0x02111508,  /* data_ov031_02111508 */
    0x00000000,
    0x35480000,
};

#pragma section dc8_section end
