/* ov014 cartridge data at 0x02113470, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113470 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113470 begin
u32 data_ov014_02113470[8] = {
    0x0a3c0229, 0x0001eb6e, 0x0ea1065c, 0x1001eb1d, 0x03e8e704, 0x3002f2a0, 0x02ee0794, 0x20021820,
};
#pragma section data_ov014_02113470 end
