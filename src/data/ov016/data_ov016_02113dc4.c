/* ov016 cartridge data at 0x02113dc4, 16 bytes. */
#include "types.h"

extern u32 data_ov016_02113b38[];
extern u32 data_ov016_02113b58[];

#pragma define_section data_ov016_02113dc4 ".data.hasira" ".data.hasira" ".data.hasira" far_data
#pragma section data_ov016_02113dc4 begin
u32 data_ov016_02113dc4[4] = {
    0x000001c0, (u32)&data_ov016_02113b38, 0x000001e0, (u32)&data_ov016_02113b58,
};
#pragma section data_ov016_02113dc4 end
