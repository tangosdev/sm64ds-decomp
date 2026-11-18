/* ov014 cartridge data at 0x02113d30, 96 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113d30 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113d30 begin
u32 data_ov014_02113d30[24] = {
    0xf0601e29, 0x05db02ed, 0xe69d1e29, 0x0dac02ee, 0xf0601e29, 0x0bb802ee, 0xf2551e29, 0x09c302ed,
    0xefb81e29, 0x122f0000, 0xfa3e1e29, 0x136e0082, 0x0b1a1e29, 0x169102ed, 0x14b41e29, 0x170c0352,
    0x100c1e29, 0x0e630389, 0x0fa01e29, 0x06400bb8, 0x19ef1e29, 0xeaaf07a8, 0x17031e29, 0xe2c7085e,
};
#pragma section data_ov014_02113d30 end
