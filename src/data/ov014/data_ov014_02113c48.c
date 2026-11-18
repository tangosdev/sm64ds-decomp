/* ov014 cartridge data at 0x02113c48, 132 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113c48 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113c48 begin
u32 data_ov014_02113c48[33] = {
    0xea840014, 0x16a80096, 0x00000001, 0x0b220000, 0x05530032, 0xf6e50b21, 0xf3340000,
    0x00010000, 0x0fa30015, 0x183702ed, 0xc0000000, 0x00000000, 0xe8c20014, 0x19640000,
    0xc00000ff, 0x07d20000, 0x10b0000f, 0xe6a40d06, 0x00000000, 0xf0f00000, 0x0bb80038,
    0xee081060, 0x982e0000, 0xff040000, 0xf4b50017, 0x00fa02ee, 0x20000000, 0xffff0000,
    0xf4280017, 0x018702ee, 0x20000000, 0xffff0000, 0xf6c70017,
};
#pragma section data_ov014_02113c48 end
