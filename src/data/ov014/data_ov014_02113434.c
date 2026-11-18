/* ov014 cartridge data at 0x02113434, 28 bytes. */
#include "types.h"

extern u32 data_ov014_021133a0[];
extern u32 data_ov014_021133b0[];
extern u32 data_ov014_021139d0[];

#pragma define_section data_ov014_02113434 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113434 begin
u32 data_ov014_02113434[7] = {
    (u32)&data_ov014_021139d0, (u32)&data_ov014_021133a0, 0x06e306e5, 0x06e706e6, (u32)&data_ov014_021133b0, 0x06400001,
    0x0000001f,
};
#pragma section data_ov014_02113434 end
