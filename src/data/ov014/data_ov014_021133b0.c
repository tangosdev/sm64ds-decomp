/* ov014 cartridge data at 0x021133b0, 12 bytes. */
#include "types.h"

extern u32 data_ov014_02113398[];

#pragma define_section data_ov014_021133b0 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021133b0 begin
u32 data_ov014_021133b0[3] = {
    (u32)&data_ov014_02113398,
    0x00000000,
    0x1f400000,
};
#pragma section data_ov014_021133b0 end
