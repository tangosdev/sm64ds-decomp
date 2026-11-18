/* ov070 data_ov070_021234c4 at 0x021234c4 (data, 24 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov070_021234c4[6] = {
    0x00640002,
    0x00000000,
    0x021227d8,  /* data_ov070_021227d8 */
    0x02122594,  /* data_ov070_02122594 */
    0x02122968,  /* data_ov070_02122968 */
    0x0212259c,  /* data_ov070_0212259c */
};

#pragma section dc8_section end
