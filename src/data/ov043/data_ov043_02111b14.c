/* ov043 cartridge data at 0x02111b14, 4 bytes. */
#include "types.h"

extern u32 data_ov043_02111b34[];

#pragma define_section data_ov043_02111b14 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b14 begin
u32 data_ov043_02111b14[1] = {
    (u32)&data_ov043_02111b34,
};
#pragma section data_ov043_02111b14 end
