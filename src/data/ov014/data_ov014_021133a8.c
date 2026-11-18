/* ov014 cartridge data at 0x021133a8, 8 bytes. */
#include "types.h"

#pragma define_section data_ov014_021133a8 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021133a8 begin
u32 data_ov014_021133a8[2] = {
    0xa0a0a001, 0xc3502710,
};
#pragma section data_ov014_021133a8 end
