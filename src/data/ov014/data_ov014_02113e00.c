/* ov014 cartridge data at 0x02113e00, 80 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113e00 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113e00 begin
u32 data_ov014_02113e00[20] = {
    0x08b4e225, 0xf84b0b20, 0xe7644a3f, 0x041a0352, 0x0b86e225, 0x04b00f3c, 0x0b86e225,
    0x05780ed8, 0x0b86e225, 0x05dc0e10, 0x0b86e225, 0x05780d48, 0x0b86e225, 0x04b00ce4,
    0x0b86e225, 0x03e80d48, 0x0b86e225, 0x03840e10, 0x0b86e225, 0x03e80ed8,
};
#pragma section data_ov014_02113e00 end
