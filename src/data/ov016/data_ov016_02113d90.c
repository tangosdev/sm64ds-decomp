/* ov016 cartridge data at 0x02113d90, 24 bytes. */
#include "types.h"

extern u32 data_ov016_02113bcc[];
extern u32 data_ov016_02113ed8[];
extern u32 data_ov016_021141b8[];

#pragma define_section data_ov016_02113d90 ".data.hasira" ".data.hasira" ".data.hasira" far_data
#pragma section data_ov016_02113d90 begin
u32 data_ov016_02113d90[6] = {
    (u32)&data_ov016_021141b8, 0x00005c05, (u32)&data_ov016_02113ed8, 0x00000220, (u32)&data_ov016_02113bcc, 0x00000240,
};
#pragma section data_ov016_02113d90 end
