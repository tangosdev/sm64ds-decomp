/* ov014 cartridge data at 0x021140b8, 52 bytes. */
#include "types.h"

#pragma define_section data_ov014_021140b8 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021140b8 begin
u32 data_ov014_021140b8[13] = {
    0x1a2c000f, 0xe9cd07ad, 0x00000000, 0xffff0000, 0x08670013, 0x0d6502e4, 0x00000000,
    0x00050000, 0xf9b90013, 0xf76e0000, 0x00000000, 0x00050000, 0xfc370013,
};
#pragma section data_ov014_021140b8 end
