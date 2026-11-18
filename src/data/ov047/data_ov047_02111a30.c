/* ov047 data_ov047_02111a30 at 0x02111a30 (data, .data_pre, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data_pre" ".data_pre" ".data_pre" far_data
#pragma section dc8_section begin
u32 data_ov047_02111a30[4] = {
    0x00003800,
    0x02111ecc,  /* data_ov047_02111ecc */
    0x00002f05,
    0x02111d54,  /* data_ov047_02111d54 */
};

#pragma section dc8_section end
