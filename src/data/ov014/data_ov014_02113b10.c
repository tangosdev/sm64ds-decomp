/* ov014 cartridge data at 0x02113b10, 28 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113b10 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113b10 begin
u32 data_ov014_02113b10[7] = {
    0x02ed07d0, 0x076c12c0, 0x0ed802ee, 0x02eef9c0, 0xf5100708, 0x044c0000, 0x0026fd44,
};
#pragma section data_ov014_02113b10 end
