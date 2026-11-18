/* 0x020d7a24: BG layer (bg, priority) pairs read by func_ov007_020b6d40.
 * u8[8][2], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020d7a24 ".rodata.menu" ".rodata.menu" ".rodata.menu" far_data
#pragma section sec_020d7a24 begin
u8 data_ov007_020d7a24[8][2] = {
    { 0x01, 0x01 },
    { 0x03, 0x01 },
    { 0x03, 0x00 },
    { 0x02, 0x03 },
    { 0x02, 0x02 },
    { 0x00, 0x01 },
    { 0x01, 0x01 },
    { 0x01, 0x00 },
};
#pragma section sec_020d7a24 end
