/* ov043 cartridge data at 0x02111b34, 16 bytes. */
#include "types.h"

extern u32 data_ov043_02111dc0[];
extern u32 data_ov043_02111fd0[];

#pragma define_section data_ov043_02111b34 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b34 begin
u32 data_ov043_02111b34[4] = {
    0x00002a00,
    (u32)&data_ov043_02111fd0,
    0x00004205,
    (u32)&data_ov043_02111dc0,
};
#pragma section data_ov043_02111b34 end
