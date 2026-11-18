/* 0x020d7c6c: menu timing table (0x3c, 0x3e8, 1, 0xa, ...).
 * u32[11], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7c6c ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c6c begin
u32 data_ov007_020d7c6c[11] = {
    0x0000003c, 0x000003e8, 0x00000001, 0x0000000a, 0x0000000a, 0x0000000a, 0x0000000a, 0x0000001e, 0x0000003c, 0x00000000, 0x000007d0,
};
#pragma section sec_020d7c6c end
