/* ov043 cartridge data at 0x02111bd4, 12 bytes. */
#include "types.h"

#pragma define_section data_ov043_02111bd4 ".data.gap2"                                            \
                                           ".data.gap2"                                            \
                                           ".data.gap2" far_data
#pragma section data_ov043_02111bd4 begin
u32 data_ov043_02111bd4[3] = {
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
#pragma section data_ov043_02111bd4 end
