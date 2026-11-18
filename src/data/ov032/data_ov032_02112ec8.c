/* ov032 cartridge data at 0x02112ec8, 4 bytes. */
#include "types.h"

#pragma define_section data_ov032_02112ec8 ".data.tdPre"                                           \
                                           ".data.tdPre"                                           \
                                           ".data.tdPre" far_data
#pragma section data_ov032_02112ec8 begin
u32 data_ov032_02112ec8[1] = {
    0x03033334,
};
#pragma section data_ov032_02112ec8 end
