/* 0x020d7c34: BG scroll rectangle keys (0xfffff000, 0x1000, ...).
 * VecFx32[3], exact cartridge words. */
#include "types.h"
#include "nitro/fx/fx.h"

#pragma define_section sec_020d7c34 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c34 begin
VecFx32 data_ov007_020d7c34[3] = {
    { 0xfffff000, 0x00001000, 0x00001000 },
    { 0xfffff000, 0x00001000, 0x00001000 },
    { 0xfffff000, 0x00001000, 0x00001000 },
};
#pragma section sec_020d7c34 end
