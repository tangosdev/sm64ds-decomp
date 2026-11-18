/* ov014 cartridge data at 0x02113b80, 16 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113b80 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113b80 begin
u32 data_ov014_02113b80[4] = {
    0x0bb8f2b8,
    0xf5100f6f,
    0x0ff6117c,
    0x1193f208,
};
#pragma section data_ov014_02113b80 end
