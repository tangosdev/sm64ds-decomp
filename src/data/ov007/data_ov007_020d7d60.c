/* 0x020d7d60: menu layout record (0, 0, 0xfffffe00).
 * VecFx32 scalar, exact cartridge words. */
#include "types.h"
#include "nitro/fx/fx.h"

#pragma define_section sec_020d7d60 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7d60 begin
VecFx32 data_ov007_020d7d60 = { 0x00000000, 0x00000000, 0xfffffe00 };
#pragma section sec_020d7d60 end
