/* ov014 cartridge data at 0x02113490, 56 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113490 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113490 begin
u32 data_ov014_02113490[14] = {
    0x0b9fff00, 0xee28110d, 0x00000000, 0xff030000, 0x10600bb8, 0x0000ee08, 0x00000000,
    0xe3abff02, 0x1c510199, 0x5e930000, 0xff070000, 0x016d0b9f, 0x0000ee28, 0x00000000,
};
#pragma section data_ov014_02113490 end
