/* ov023 data_ov023_02111b28 at 0x02111b28 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov023_02111b28[2] = {
    0x00000002,
    0x02111b60,  /* data_ov023_02111b60 */
};

#pragma section dc8_section end
