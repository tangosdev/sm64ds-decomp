/* ov014 cartridge data at 0x021140f8, 16 bytes. */
#include "types.h"

#pragma define_section data_ov014_021140f8 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021140f8 begin
u32 data_ov014_021140f8[4] = {
    0xf5d80013, 0x06400000, 0x00000000, 0x00050000,
};
#pragma section data_ov014_021140f8 end
