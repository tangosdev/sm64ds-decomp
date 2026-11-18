/* ov070 data_ov070_021234dc at 0x021234dc (data, 36 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov070_021234dc[9] = {
    0x00460002,
    0x00000000,
    0x021226bc,  /* data_ov070_021226bc */
    0x0212262c,  /* data_ov070_0212262c */
    0x02122598,  /* data_ov070_02122598 */
    0x021225e4,  /* data_ov070_021225e4 */
    0x00000000,
    0x00000000,
    0x00000000,
};

#pragma section dc8_section end
