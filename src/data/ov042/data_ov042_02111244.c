/* ov042 data_ov042_02111244 at 0x02111244 (data, 32 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov042_02111244[8] = {
    0x00000401,
    0x021112a4,  /* data_ov042_021112a4 */
    0x0000010c,
    0x021111c0,  /* data_ov042_021111c0 */
    0x0000010e,
    0x021111c8,  /* data_ov042_021111c8 */
    0x0000010a,
    0x021111f8,  /* data_ov042_021111f8 */
};

#pragma section dc8_section end
