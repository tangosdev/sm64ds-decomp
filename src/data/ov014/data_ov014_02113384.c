/* ov014 cartridge data at 0x02113384, 4 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113384 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113384 begin
u32 data_ov014_02113384[1] = {
    0x00010114,
};
#pragma section data_ov014_02113384 end
