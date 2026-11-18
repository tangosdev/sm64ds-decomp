/* ov014 cartridge data at 0x02113800, 4 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113800 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113800 begin
u32 data_ov014_02113800[1] = {
    0x0190000a,
};
#pragma section data_ov014_02113800 end
