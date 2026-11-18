/* ov016 cartridge data at 0x02113e00, 80 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113e00 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02113e00 begin
u32 data_ov016_02113e00[20] = {
    0x000000ff, 0x00042fc8, 0x000000ff, 0x00042fc4, 0x000000ff, 0x01000fc0, 0x000000ff,
    0x00000fe0, 0x000000ff, 0x01200fc0, 0x000000ff, 0x00030fc4, 0x000000ff, 0x00030fc5,
    0x000000ff, 0x01880fcc, 0x000000ff, 0xffffffff, 0xffffffff, 0xffffffff,
};
#pragma section data_ov016_02113e00 end
