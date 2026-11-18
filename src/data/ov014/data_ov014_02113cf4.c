/* ov014 cartridge data at 0x02113cf4, 20 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113cf4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113cf4 begin
u32 data_ov014_02113cf4[5] = {
    0xffff0000, 0x0207000f, 0xf5be0ac9, 0x80000000, 0xf2f00000,
};
#pragma section data_ov014_02113cf4 end
