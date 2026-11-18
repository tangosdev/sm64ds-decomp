/* ov014 cartridge data at 0x021133a0, 8 bytes. */
#include "types.h"

extern u32 data_ov014_02113508[];

#pragma define_section data_ov014_021133a0 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021133a0 begin
u32 data_ov014_021133a0[2] = {
    0x00000009,
    (u32)&data_ov014_02113508,
};
#pragma section data_ov014_021133a0 end
