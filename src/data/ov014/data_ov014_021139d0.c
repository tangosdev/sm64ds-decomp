/* ov014 cartridge data at 0x021139d0, 36 bytes. */
#include "types.h"

#pragma define_section data_ov014_021139d0 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_021139d0 begin
u32 data_ov014_021139d0[9] = {
    0x53504c43, 0x00180008, 0x00000fc0, 0x000000ff, 0x00040fc2, 0x000000ff,
    0x00040fc1, 0x000000ff, 0x00038042,
};
#pragma section data_ov014_021139d0 end
