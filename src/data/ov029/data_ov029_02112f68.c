/* ov029 cartridge data at 0x02112f68, 16 bytes. */
#include "types.h"

#pragma define_section data_ov029_02112f68 ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112f68 begin
u32 data_ov029_02112f68[4] = {
    0x11300017,
    0x03e80000,
    0x00000000,
    0xffff0000,
};
#pragma section data_ov029_02112f68 end
