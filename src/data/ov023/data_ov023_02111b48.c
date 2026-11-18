/* ov023 data_ov023_02111b48 at 0x02111b48 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov023_02111b48[3] = {
    0x02111b28,  /* data_ov023_02111b28 */
    0x02111b70,  /* data_ov023_02111b70 */
    0x1c200000,
};

#pragma section dc8_section end
