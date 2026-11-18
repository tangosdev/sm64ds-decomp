/* ov016 cartridge data at 0x02113c28, 4 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113c28 ".data.hasira_mid"                                      \
                                           ".data.hasira_mid"                                      \
                                           ".data.hasira_mid" far_data
#pragma section data_ov016_02113c28 begin
u32 data_ov016_02113c28[1] = {
    0x0b260000,
};
#pragma section data_ov016_02113c28 end
