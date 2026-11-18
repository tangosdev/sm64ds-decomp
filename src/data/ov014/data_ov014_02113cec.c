/* ov014 cartridge data at 0x02113cec, 8 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113cec ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113cec begin
u32 data_ov014_02113cec[2] = {
    0xfd7a03e8,
    0x20000000,
};
#pragma section data_ov014_02113cec end
