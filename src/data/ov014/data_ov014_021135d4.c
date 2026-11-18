/* ov014 cartridge data at 0x021135d4, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_021135d4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021135d4 begin
u32 data_ov014_021135d4[8] = {
    0x16a80096, 0x00000001, 0x0b220000, 0x05530032, 0xf6e50b21, 0xf3340000, 0x00010000, 0xe8c20014,
};
#pragma section data_ov014_021135d4 end
