/* ov014 cartridge data at 0x021142a0, 156 bytes. */
#include "types.h"

#pragma define_section data_ov014_021142a0 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021142a0 begin
u32 data_ov014_021142a0[39] = {
    0xc0000000, 0x00020000, 0x10360032, 0xf6a00bb8, 0xc0000000, 0x00020000, 0x109a0032, 0x04b10bb8,
    0x80000000, 0x0002c000, 0x0e74001e, 0x079e0cb2, 0x00000000, 0xff050000, 0x0e34004f, 0xe5390d7f,
    0x00000000, 0xffff0000, 0xfa240130, 0x04b0154a, 0x00000000, 0x0f050000, 0x0b860130, 0x04b00e10,
    0x00000000, 0x0f050000, 0x08340130, 0x04b00e10, 0x00000000, 0x0f050000, 0x04e20130, 0x04b00e10,
    0x00000000, 0x0f050000, 0xfe3e0130, 0x04b01004, 0x00000000, 0x0f050000, 0xee9e0013,
};
#pragma section data_ov014_021142a0 end
