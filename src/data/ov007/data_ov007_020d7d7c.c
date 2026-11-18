/* 0x020d7d7c: menu u16 table (8, 0x10, 0x20, 0x40, 0x10, 0x20, ...).
 * u16[3][4], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7d7c ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7d7c begin
u16 data_ov007_020d7d7c[3][4] = {
    { 0x0008, 0x0010, 0x0020, 0x0040 },
    { 0x0010, 0x0020, 0x0020, 0x0040 },
    { 0x0008, 0x0008, 0x0010, 0x0020 },
};
#pragma section sec_020d7d7c end
