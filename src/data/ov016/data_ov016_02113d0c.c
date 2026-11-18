/* ov016 cartridge data at 0x02113d0c, 36 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113d0c ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02113d0c begin
u32 data_ov016_02113d0c[9] = {
    0xe85e0000, 0xff060af0, 0x2aaa0000, 0x05000000, 0xe7960000,
    0xfe0c044c, 0x1c710000, 0x00000000, 0xe7000000,
};
#pragma section data_ov016_02113d0c end
