/* ov014 cartridge data at 0x02113910, 36 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113910 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113910 begin
u32 data_ov014_02113910[9] = {
    0xf1a70034, 0xf9e00b22, 0x00000000, 0x00000000, 0x0ea00034,
    0x0d8e0b22, 0x00000000, 0x00000000, 0x05530032,
};
#pragma section data_ov014_02113910 end
