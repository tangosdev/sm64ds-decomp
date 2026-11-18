/* ov057 data_ov057_02111520 at 0x02111520 (data, 28 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov057_02111520[7] = {
    0x0211157c,  /* data_ov057_0211157c */
    0x021114b4,  /* data_ov057_021114b4 */
    0x07360738,
    0x073a0739,
    0x021114cc,  /* data_ov057_021114cc */
    0x07080001,
    0x0000004f,
};

#pragma section dc8_section end
