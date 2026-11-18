/* ov014 cartridge data at 0x02113b2c, 36 bytes. */
#include "types.h"

#pragma define_section data_ov014_02113b2c ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113b2c begin
u32 data_ov014_02113b2c[9] = {
    0x0b54fda8, 0xfaec0634, 0x0779189c, 0x170cf254, 0xe638080c,
    0x09630bb8, 0xfe0ce37c, 0xebb009f8, 0x0a4f00c8,
};
#pragma section data_ov014_02113b2c end
