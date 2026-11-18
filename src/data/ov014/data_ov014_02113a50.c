/* ov014 cartridge data at 0x02113a50, 36 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113a50 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113a50 begin
u32 data_ov014_02113a50[9] = {
    0x001b0fc5, 0x000000ff, 0x01030fc4, 0x000000ff, 0x01030fcc, 0x000000ff,
    0x00030fc9, 0x000000ff, 0x00030fc5,
};
#pragma section data_ov014_02113a50 end
