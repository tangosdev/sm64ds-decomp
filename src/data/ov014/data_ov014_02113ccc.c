/* ov014 cartridge data at 0x02113ccc, 8 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113ccc ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113ccc begin
u32 data_ov014_02113ccc[2] = {
    0xedb003b8,
    0x13330000,
};
#pragma section data_ov014_02113ccc end
