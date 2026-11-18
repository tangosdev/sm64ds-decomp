/* ov043 cartridge data at 0x02111b10, 4 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111b10 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b10 begin
u32 data_ov043_02111b10[1] = {
    0x00000002,
};
#pragma section data_ov043_02111b10 end
