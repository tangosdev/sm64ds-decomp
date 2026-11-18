/* ov016 cartridge data at 0x021140f8, 16 bytes. */
#include "types.h"

#pragma define_section data_ov016_021140f8 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_021140f8 begin
u32 data_ov016_021140f8[4] = {
    0xebb02227,
    0x0a5a03e8,
    0xeed02227,
    0x0d7a03e8,
};
#pragma section data_ov016_021140f8 end
