/* ov038 data_ov038_02111208 at 0x02111208 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov038_02111208[2] = {
    0x00000200,
    0x02111234,  /* data_ov038_02111234 */
};

#pragma section dc8_section end
