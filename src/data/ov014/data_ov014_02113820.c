/* ov014 cartridge data at 0x02113820, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113820 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113820 begin
u32 data_ov014_02113820[3] = {
    0x05530032,
    0xf6e50b21,
    0xf3340000,
};
#pragma section data_ov014_02113820 end
