/* ov014 cartridge data at 0x021136d4, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_021136d4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_021136d4 begin
u32 data_ov014_021136d4[8] = {
    0xfd1203e8, 0x00000000, 0xffff0000, 0xef330017, 0xfd7a03e8, 0x20000000, 0xffff0000, 0xff38000f,
};
#pragma section data_ov014_021136d4 end
