/* ov034 cartridge data at 0x02113820, 8 bytes. */
#include "types.h"

#pragma define_section data_ov034_02113820 ".rodata"                                               \
                                           ".rodata"                                               \
                                           ".rodata" far_data
#pragma section data_ov034_02113820 begin
short data_ov034_02113820[4] = {
    0x00c6,
    0x00c7,
    0x00c8,
    0x00c9,
};
#pragma section data_ov034_02113820 end
