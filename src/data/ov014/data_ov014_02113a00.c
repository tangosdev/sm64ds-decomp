/* ov014 cartridge data at 0x02113a00, 80 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113a00 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113a00 begin
u32 data_ov014_02113a00[20] = {
    0x01040004, 0x000000ff, 0x00040005, 0x000000ff, 0x00040004, 0x000000ff,
    0x00040002, 0x000000ff, 0x00040001, 0x000000ff, 0x00042002, 0x000000ff,
    0x00042001, 0x000000ff, 0x00043002, 0x000000ff, 0x00043001, 0x000000ff,
    0x00030fc1, 0x000000ff,
};
#pragma section data_ov014_02113a00 end
