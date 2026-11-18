/* 0x020cccf0: BTA rotation keys of data_ov007_020cdabc (all zero).
 * u16[2], exact cartridge bytes. */
#include "types.h"

#pragma explicit_zero_data on
#pragma define_section sec_020cccf0 ".rodata.gap6" ".rodata.gap6" ".rodata.gap6" far_data
#pragma section sec_020cccf0 begin
u16 data_ov007_020cccf0[2] = {
    0x0000, 0x0000,
};
#pragma section sec_020cccf0 end
