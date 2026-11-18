/* ov014 cartridge data at 0x02114578, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_02114578 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02114578 begin
u32 data_ov014_02114578[8] = {
    0x53504c43, 0x00010008, 0x01880fc0, 0x000000ff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff,
};
#pragma section data_ov014_02114578 end
