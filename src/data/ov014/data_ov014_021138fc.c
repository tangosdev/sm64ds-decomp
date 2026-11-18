/* ov014 cartridge data at 0x021138fc, 20 bytes. */
#include "types.h"

#pragma define_section data_ov014_021138fc ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021138fc begin
u32 data_ov014_021138fc[5] = {
    0xf0f00000, 0x1451000f, 0x0e1803cf, 0xe0000000, 0xf1f00000,
};
#pragma section data_ov014_021138fc end
