/* ov042 data_ov042_02111234 at 0x02111234 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov042_02111234[4] = {
    0x00000104,
    0x021111e8,  /* data_ov042_021111e8 */
    0x0000010b,
    0x021111c4,  /* data_ov042_021111c4 */
};

#pragma section dc8_section end
