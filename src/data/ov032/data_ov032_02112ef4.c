/* ov032 cartridge data at 0x02112ef4, 16 bytes. */
#include "types.h"

#pragma define_section data_ov032_02112ef4 ".data.w12ee8"                                          \
                                           ".data.w12ee8"                                          \
                                           ".data.w12ee8" far_data
#pragma section data_ov032_02112ef4 begin
u32 data_ov032_02112ef4[4] = {
    0xe63800ef,
    0x0384f736,
    0x00000000,
    0xffff0000,
};
#pragma section data_ov032_02112ef4 end
