/* 0x020d7d94: menu u16 table (8, 0x10, 0x20, 0x40, 8, 8, 0x10, ...).
 * u16[3][4], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7d94 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7d94 begin
u16 data_ov007_020d7d94[3][4] = {
    { 0x0008, 0x0010, 0x0020, 0x0040 },
    { 0x0008, 0x0008, 0x0010, 0x0020 },
    { 0x0010, 0x0020, 0x0020, 0x0040 },
};
#pragma section sec_020d7d94 end
