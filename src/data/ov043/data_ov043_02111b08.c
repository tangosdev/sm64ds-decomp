/* ov043 cartridge data at 0x02111b08, 8 bytes. */
#include "types.h"

extern u32 data_ov043_02111ca0[];

#pragma define_section data_ov043_02111b08 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b08 begin
u32 data_ov043_02111b08[2] = {
    0x00000005,
    (u32)&data_ov043_02111ca0,
};
#pragma section data_ov043_02111b08 end
