/* ov014 cartridge data at 0x021139b0, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_021139b0 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021139b0 begin
u32 data_ov014_021139b0[8] = {
    0xff38000f, 0x107c0000, 0xc0000000, 0xf0f00000, 0x1451000f, 0x0e1803cf,
    0xe0000000, 0xf1f00000,
};
#pragma section data_ov014_021139b0 end
