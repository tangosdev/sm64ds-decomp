/* ov014 cartridge data at 0x02113998, 24 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113998 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113998 begin
u32 data_ov014_02113998[6] = {
    0x60000000, 0x00000000, 0x04620013, 0xf6920b10, 0xf3340000, 0x00010000,
};
#pragma section data_ov014_02113998 end
