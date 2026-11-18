/* 0x020d7d60: menu layout record (0, 0, 0xfffffe00).
 * u8[12], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7d60 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7d60 begin
u8 data_ov007_020d7d60[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0xff,
};
#pragma section sec_020d7d60 end
