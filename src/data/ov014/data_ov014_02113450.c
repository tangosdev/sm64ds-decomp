/* ov014 cartridge data at 0x02113450, 32 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113450 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113450 begin
u32 data_ov014_02113450[8] = {
    0x0ea1063f, 0x0090eae8, 0x0a3c0207, 0x0090eb2a, 0x03e8e713, 0x00c0f28b, 0x02ee0752, 0x000017fe,
};
#pragma section data_ov014_02113450 end
