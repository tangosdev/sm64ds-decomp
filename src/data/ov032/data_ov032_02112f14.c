/* ov032 cartridge data at 0x02112f14, 16 bytes. */
#include "types.h"

#pragma define_section data_ov032_02112f14 ".data.w12ee8"                                          \
                                           ".data.w12ee8"                                          \
                                           ".data.w12ee8" far_data
#pragma section data_ov032_02112f14 begin
u32 data_ov032_02112f14[4] = {
    0xe63800ef,
    0x0384f736,
    0x00000000,
    0xffff0000,
};
#pragma section data_ov032_02112f14 end
