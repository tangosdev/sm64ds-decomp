/* ov014 cartridge data at 0x02113398, 8 bytes. */
#include "types.h"

extern u32 data_ov014_02113550[];

#pragma define_section data_ov014_02113398 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113398 begin
u32 data_ov014_02113398[2] = {
    0x00000010,
    (u32)&data_ov014_02113550,
};
#pragma section data_ov014_02113398 end
