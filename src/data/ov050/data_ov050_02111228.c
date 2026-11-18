/* ov050 data_ov050_02111228 at 0x02111228 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111228[7] = {
    0x00000203,
    0x021111e0,  /* data_ov050_021111e0 */
    0x00000404,
    0x021112a0,  /* data_ov050_021112a0 */
    0x0000010b,
    0x021111c0,  /* data_ov050_021111c0 */
    0x00000401,
};

#pragma section dc8_section end
