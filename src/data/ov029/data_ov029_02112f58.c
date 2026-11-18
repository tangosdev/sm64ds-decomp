/* ov029 cartridge data at 0x02112f58, 16 bytes. */
#include "types.h"

#pragma define_section data_ov029_02112f58 ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112f58 begin
u32 data_ov029_02112f58[4] = {
    0x11300017,
    0x03e80000,
    0x00003e93,
    0xffff0000,
};
#pragma section data_ov029_02112f58 end
