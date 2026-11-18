/* ov028 data_ov028_02111364 at 0x02111364 (data, 12 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov028_02111364[3] = {
    0x02111348,  /* data_ov028_02111348 */
    0x021113b8,  /* data_ov028_021113b8 */
    0x02580000,
};

#pragma section dc8_section end
