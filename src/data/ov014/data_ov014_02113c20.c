/* ov014 cartridge data at 0x02113c20, 8 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113c20 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113c20 begin
u32 data_ov014_02113c20[2] = {
    0x00020005,
    0x0022ffff,
};
#pragma section data_ov014_02113c20 end
