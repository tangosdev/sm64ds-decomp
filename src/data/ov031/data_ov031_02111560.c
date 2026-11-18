/* ov031 data_ov031_02111560 at 0x02111560 (data, 24 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov031_02111560[6] = {
    0x02111510,  /* data_ov031_02111510 */
    0x07550757,
    0x07590758,
    0x02111520,  /* data_ov031_02111520 */
    0x0af00001,
    0x0000000f,
};

#pragma section dc8_section end
