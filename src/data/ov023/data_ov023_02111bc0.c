/* ov023 data_ov023_02111bc0 at 0x02111bc0 (data, 20 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov023_02111bc0[5] = {
    0x00001902,
    0x02111c98,  /* data_ov023_02111c98 */
    0x00000203,
    0x02111b54,  /* data_ov023_02111b54 */
    0x00000204,
};

#pragma section dc8_section end
