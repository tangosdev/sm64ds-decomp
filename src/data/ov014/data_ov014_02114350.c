/* ov014 cartridge data at 0x02114350, 48 bytes. */
#include "types.h"

#pragma define_section data_ov014_02114350 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02114350 begin
u32 data_ov014_02114350[12] = {
    0x00000000, 0x00f30000, 0x1853000f, 0x023402e7, 0x00000000, 0xffff0000,
    0xea46001d, 0x155c007d, 0x00000000, 0xff010000, 0x1482001d, 0x119405dc,
};
#pragma section data_ov014_02114350 end
