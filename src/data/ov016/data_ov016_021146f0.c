/* ov016 cartridge data at 0x021146f0, 96 bytes. */
#include "types.h"

#pragma define_section data_ov016_021146f0 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_021146f0 begin
u32 data_ov016_021146f0[24] = {
    0x60000000, 0xffff0000, 0x0000002c, 0xe4daf4af, 0x40000000, 0x17060000, 0x13ec001e, 0xe5d4f5a9,
    0x00000000, 0xff060000, 0xef34004c, 0xff6a044c, 0xc000ffff, 0x074f0000, 0x186a00fe, 0x08fcec78,
    0xf1c80000, 0x11040000, 0x167600fe, 0xe3e0f4af, 0xeaab0000, 0x01040000, 0x015e00fe, 0x0b54fd6c,
};
#pragma section data_ov016_021146f0 end
