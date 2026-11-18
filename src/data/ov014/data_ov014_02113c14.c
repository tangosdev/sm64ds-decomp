/* ov014 cartridge data at 0x02113c14, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113c14 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113c14 begin
u32 data_ov014_02113c14[3] = {
    0xffc003e2,
    0xf66e013b,
    0xf128035d,
};
#pragma section data_ov014_02113c14 end
