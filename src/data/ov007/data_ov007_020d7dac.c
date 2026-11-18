/* 0x020d7dac: menu layout record (0, 0, 0x1000).
 * u8[12], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7dac ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7dac begin
u8 data_ov007_020d7dac[12] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
};
#pragma section sec_020d7dac end
