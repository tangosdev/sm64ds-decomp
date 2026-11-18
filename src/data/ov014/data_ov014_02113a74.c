/* ov014 cartridge data at 0x02113a74, 44 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113a74 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113a74 begin
u32 data_ov014_02113a74[11] = {
    0x000000ff, 0x00030fc4, 0x000000ff, 0x00030fc2, 0x000000ff, 0x00033fc2,
    0x000000ff, 0x00033fc4, 0x000000ff, 0xffffffff, 0xffffffff,
};
#pragma section data_ov014_02113a74 end
