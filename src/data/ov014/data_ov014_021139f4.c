/* ov014 cartridge data at 0x021139f4, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_021139f4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021139f4 begin
u32 data_ov014_021139f4[3] = {
    0x000000ff,
    0x00038041,
    0x000000ff,
};
#pragma section data_ov014_021139f4 end
