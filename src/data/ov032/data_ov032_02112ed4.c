/* ov032 cartridge data at 0x02112ed4, 8 bytes. */
#include "types.h"

extern u32 data_ov032_02113040[];

#pragma define_section data_ov032_02112ed4 ".data.tdPre"                                           \
                                           ".data.tdPre"                                           \
                                           ".data.tdPre" far_data
#pragma section data_ov032_02112ed4 begin
u32 data_ov032_02112ed4[2] = {
    0x00000008,
    (u32)&data_ov032_02113040,
};
#pragma section data_ov032_02112ed4 end
