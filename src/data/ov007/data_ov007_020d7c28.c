/* 0x020d7c28: BG scroll limits.
 * u8[12], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7c28 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c28 begin
u8 data_ov007_020d7c28[12] = {
    0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xb0, 0xff, 0xff,
};
#pragma section sec_020d7c28 end
