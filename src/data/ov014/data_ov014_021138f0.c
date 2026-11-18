/* ov014 cartridge data at 0x021138f0, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_021138f0 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021138f0 begin
u32 data_ov014_021138f0[3] = {
    0xff38000f, 0x107c0000, 0xc0000000,
};
#pragma section data_ov014_021138f0 end
