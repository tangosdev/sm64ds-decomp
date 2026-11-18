/* ov049 data_ov049_021111ec at 0x021111ec (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov049_021111ec[4] = {
    0x00000a03,
    0x02111238,  /* data_ov049_02111238 */
    0x00000401,
    0x02111274,  /* data_ov049_02111274 */
};

#pragma section dc8_section end
