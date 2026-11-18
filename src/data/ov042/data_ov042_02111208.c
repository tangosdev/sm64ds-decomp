/* ov042 data_ov042_02111208 at 0x02111208 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov042_02111208[2] = {
    0x00001c00,
    0x02111404,  /* data_ov042_02111404 */
};

#pragma section dc8_section end
