/* 0x020cccf8: BTA rotation keys of data_ov007_020cd9e4 (all zero).
 * u16[2], exact cartridge bytes. */
#include "types.h"

#pragma explicit_zero_data on
#pragma define_section sec_020cccf8 ".rodata.gap7" ".rodata.gap7" ".rodata.gap7" far_data
#pragma section sec_020cccf8 begin
u16 data_ov007_020cccf8[2] = {
    0x0000, 0x0000,
};
#pragma section sec_020cccf8 end
