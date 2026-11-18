/* ov032 cartridge data at 0x02112ecc, 8 bytes. */
#include "types.h"

extern u32 data_ov032_02113008[];

#pragma define_section data_ov032_02112ecc ".data.tdPre" ".data.tdPre" ".data.tdPre" far_data
#pragma section data_ov032_02112ecc begin
u32 data_ov032_02112ecc[2] = {
    0x00000007, (u32)&data_ov032_02113008,
};
#pragma section data_ov032_02112ecc end
