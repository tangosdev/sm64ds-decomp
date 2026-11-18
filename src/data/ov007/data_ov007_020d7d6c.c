/* 0x020d7d6c: menu bit masks (1, 2, 4, 8).
 * u32[4], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7d6c ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7d6c begin
u32 data_ov007_020d7d6c[4] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
};
#pragma section sec_020d7d6c end
