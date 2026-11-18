/* ov014 cartridge data at 0x021138a8, 20 bytes. */
#include "types.h"

#pragma define_section data_ov014_021138a8 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021138a8 begin
u32 data_ov014_021138a8[5] = {
    0xc0000000, 0x00000000, 0xe8c20014, 0x19640000, 0xc00000ff,
};
#pragma section data_ov014_021138a8 end
