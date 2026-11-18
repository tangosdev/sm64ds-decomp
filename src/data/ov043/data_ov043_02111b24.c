/* ov043 cartridge data at 0x02111b24, 16 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111b24 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b24 begin
u32 data_ov043_02111b24[4] = {
    0x0b2219ff,
    0xc000005d,
    0x00240000,
    0x00001104,
};
#pragma section data_ov043_02111b24 end
