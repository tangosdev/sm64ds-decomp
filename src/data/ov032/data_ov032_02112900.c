/* ov032 cartridge data at 0x02112900, 28 bytes. */
#include "types.h"

extern char data_ov032_02112edc[];

#pragma define_section data_ov032_02112900 ".rodata.w2900"                                         \
                                           ".rodata.w2900"                                         \
                                           ".rodata.w2900" far_data
#pragma section data_ov032_02112900 begin
u32 data_ov032_02112900[7] = {
    0x0000ffff, (u32)&data_ov032_02112edc, 0x00000001, 0x00000001, 0x00000001, 0x0000005b,
    0x0000005b,
};
#pragma section data_ov032_02112900 end
