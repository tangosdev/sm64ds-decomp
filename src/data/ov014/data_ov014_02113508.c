/* ov014 cartridge data at 0x02113508, 16 bytes. */
#include "types.h"

extern u32 data_ov014_021133ec[];
extern u32 data_ov014_02113ab0[];

#pragma define_section data_ov014_02113508 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113508 begin
u32 data_ov014_02113508[4] = {
    0x00003102, (u32)&data_ov014_02113ab0, 0x00000403, (u32)&data_ov014_021133ec,
};
#pragma section data_ov014_02113508 end
