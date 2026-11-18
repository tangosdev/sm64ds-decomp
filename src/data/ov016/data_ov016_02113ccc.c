/* ov016 cartridge data at 0x02113ccc, 8 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113ccc ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02113ccc begin
u32 data_ov016_02113ccc[2] = {
    0x53504c43,
    0x00010008,
};
#pragma section data_ov016_02113ccc end
