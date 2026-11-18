/* 0x020d7c98: single menu constant (0).
 * u8[4], exact cartridge bytes. */
#include "types.h"

#pragma explicit_zero_data on
#pragma define_section sec_020d7c98 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c98 begin
u8 data_ov007_020d7c98[4] = {
    0x00, 0x00, 0x00, 0x00,
};
#pragma section sec_020d7c98 end
