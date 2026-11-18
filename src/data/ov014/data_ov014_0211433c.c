/* ov014 cartridge data at 0x0211433c, 20 bytes. */
#include "types.h"

#pragma define_section data_ov014_0211433c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_0211433c begin
u32 data_ov014_0211433c[5] = {
    0xf76803e8, 0x00000000, 0x00050000, 0xf6d20031, 0xf02e0000,
};
#pragma section data_ov014_0211433c end
