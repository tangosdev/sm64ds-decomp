/* ov014 cartridge data at 0x021140ec, 12 bytes. */
#include "types.h"

#pragma define_section data_ov014_021140ec ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021140ec begin
u32 data_ov014_021140ec[3] = {
    0xfc170000,
    0x00000000,
    0x00050000,
};
#pragma section data_ov014_021140ec end
