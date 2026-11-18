/* 0x020ccc44: BG map palette indices: [0] unselected mode, [1] selected mode.
 * u8[4], exact cartridge bytes. */
#include "types.h"

#pragma define_section sec_020ccc44 ".rodata" ".rodata" ".rodata" far_data
#pragma section sec_020ccc44 begin
u8 data_ov007_020ccc44[4] = {
    0x03, 0x04, 0x00, 0x00,
};
#pragma section sec_020ccc44 end
