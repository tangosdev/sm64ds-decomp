/* 0x020cccd8: BTA rotation keys of data_ov007_020cdb04 (all zero).
 * u16[2], exact cartridge bytes. */
#include "types.h"

#pragma explicit_zero_data on
#pragma define_section sec_020cccd8 ".rodata.gap4" ".rodata.gap4" ".rodata.gap4" far_data
#pragma section sec_020cccd8 begin
u16 data_ov007_020cccd8[2] = {
    0x0000, 0x0000,
};
#pragma section sec_020cccd8 end
