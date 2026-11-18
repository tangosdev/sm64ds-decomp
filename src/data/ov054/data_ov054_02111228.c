/* ov054 data_ov054_02111228 at 0x02111228 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov054_02111228[4] = {
    0x00001f00,
    0x021118f8,  /* data_ov054_021118f8 */
    0x00000305,
    0x02111280,  /* data_ov054_02111280 */
};

#pragma section dc8_section end
