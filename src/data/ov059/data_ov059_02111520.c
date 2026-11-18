/* ov059 data_ov059_02111520 at 0x02111520 (data, 16 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov059_02111520[4] = {
    0x00000f00,
    0x02111818,  /* data_ov059_02111818 */
    0x00001905,
    0x02111750,  /* data_ov059_02111750 */
};

#pragma section dc8_section end
