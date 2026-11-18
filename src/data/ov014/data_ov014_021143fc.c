/* ov014 cartridge data at 0x021143fc, 92 bytes. */
#include "types.h"

#pragma define_section data_ov014_021143fc ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021143fc begin
u32 data_ov014_021143fc[23] = {
    0xed4003e8, 0x00000000, 0x00fb0000, 0xe7fa0031, 0xebe203e8, 0x00000000, 0x00850000, 0xe6ce0031,
    0xed4003e8, 0x00000000, 0x00fb0000, 0xe6380031, 0xec7803e8, 0x00000000, 0x00fb0000, 0xe66a0031,
    0xeb7e03e8, 0x00000000, 0x00fb0000, 0xe7960031, 0xeab603e8, 0x00000000, 0x00fb0000,
};
#pragma section data_ov014_021143fc end
