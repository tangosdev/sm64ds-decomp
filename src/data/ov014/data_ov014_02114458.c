/* ov014 cartridge data at 0x02114458, 64 bytes. */
#include "types.h"

#pragma define_section data_ov014_02114458 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02114458 begin
u32 data_ov014_02114458[16] = {
    0xe8c20031, 0xea8403e8, 0x00000000, 0x00fb0000, 0xe9ee0031, 0xeb4c03e8, 0x00000000, 0x00fb0000,
    0xe9580031, 0xec7803e8, 0x00000000, 0x00fb0000, 0xf4480013, 0xec460000, 0x00000000, 0x00050000,
};
#pragma section data_ov014_02114458 end
