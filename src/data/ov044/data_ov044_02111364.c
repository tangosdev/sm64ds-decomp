/* ov044 data_ov044_02111364 at 0x02111364 (data, 8 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data" ".data" ".data" far_data
#pragma section dc8_section begin
u32 data_ov044_02111364[2] = {
    0x00001900,
    0x0211142c,  /* data_ov044_0211142c */
};

#pragma section dc8_section end
