/* 0x020d7c28: BG scroll limits.
 * VecFx32 scalar, exact cartridge words. */
#include "types.h"
#include "nitro/fx/fx.h"

#pragma define_section sec_020d7c28 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7c28 begin
VecFx32 data_ov007_020d7c28 = { 0x0000f000, 0xffff0000, 0xffffb000 };
#pragma section sec_020d7c28 end
