/* ov029 cartridge data at 0x02112f2c, 12 bytes. */
#include "types.h"

#pragma define_section data_ov029_02112f2c ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112f2c begin
u32 data_ov029_02112f2c[3] = {
    0x03e80000,
    0x00000000,
    0xffff0000,
};
#pragma section data_ov029_02112f2c end
