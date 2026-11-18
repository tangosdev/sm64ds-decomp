/* ov016 cartridge data at 0x02114860, 20 bytes. */
#include "types.h"

#pragma define_section data_ov016_02114860 ".data.hasira" ".data.hasira" ".data.hasira" far_data
#pragma section data_ov016_02114860 begin
u32 data_ov016_02114860[5] = {
    0x20000000, 0xff000000, 0x00c8004c, 0xe3e0f4af, 0x4000ffff,
};
#pragma section data_ov016_02114860 end
