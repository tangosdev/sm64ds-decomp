/* ov032 cartridge data at 0x02112f24, 8 bytes. */
#include "types.h"

#pragma define_section data_ov032_02112f24 ".data.w12ee8"                                          \
                                           ".data.w12ee8"                                          \
                                           ".data.w12ee8" far_data
#pragma section data_ov032_02112f24 begin
u32 data_ov032_02112f24[2] = {
    0xe63800ef,
    0x0384f736,
};
#pragma section data_ov032_02112f24 end
