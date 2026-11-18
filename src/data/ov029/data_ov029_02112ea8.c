/* ov029 cartridge data at 0x02112ea8, 8 bytes. */
#include "types.h"

extern u32 data_ov029_02112f88[];

#pragma define_section data_ov029_02112ea8 ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112ea8 begin
u32 data_ov029_02112ea8[2] = {
    0x00000003,
    (u32)&data_ov029_02112f88,
};
#pragma section data_ov029_02112ea8 end
