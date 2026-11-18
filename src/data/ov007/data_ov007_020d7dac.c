/* 0x020d7dac: menu layout record (0, 0, 0x1000).
 * VecFx32 scalar, exact cartridge words. */
#include "types.h"
#include "nitro/fx/fx.h"

#pragma define_section sec_020d7dac ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7dac begin
VecFx32 data_ov007_020d7dac = { 0x00000000, 0x00000000, 0x00001000 };
#pragma section sec_020d7dac end
