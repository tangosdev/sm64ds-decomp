/* 0x020d7c34: BG scroll rectangle keys (0xfffff000, 0x1000, ...).
 * u32[9], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7c34 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c34 begin
u32 data_ov007_020d7c34[9] = {
    0xfffff000, 0x00001000, 0x00001000, 0xfffff000, 0x00001000, 0x00001000, 0xfffff000, 0x00001000, 0x00001000,
};
#pragma section sec_020d7c34 end
