/* ov014 cartridge data at 0x0211454c, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_0211454c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_0211454c begin
u32 data_ov014_0211454c[3] = {
    0x190b0000,
    0x00000000,
    0xffff0000,
};
#pragma section data_ov014_0211454c end
