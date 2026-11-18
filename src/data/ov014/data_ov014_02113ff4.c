/* ov014 cartridge data at 0x02113ff4, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113ff4 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113ff4 begin
u32 data_ov014_02113ff4[7] = {
    0x00000000, 0x01a7000d, 0xeacc0a3c, 0x97780000, 0x01200000, 0x05c5000d, 0xea6a0ea1,
};
#pragma section data_ov014_02113ff4 end
