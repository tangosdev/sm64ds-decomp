/* ov050 data_ov050_02111248 at 0x02111248 (data, 40 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov050_02111248[10] = {
    0x00000300,
    0x02111270,  /* data_ov050_02111270 */
    0x00002e20,
    0x02111570,  /* data_ov050_02111570 */
    0x00001d25,
    0x02111488,  /* data_ov050_02111488 */
    0x00000840,
    0x02111378,  /* data_ov050_02111378 */
    0x00001245,
    0x021113f8,  /* data_ov050_021113f8 */
};

#pragma section dc8_section end
