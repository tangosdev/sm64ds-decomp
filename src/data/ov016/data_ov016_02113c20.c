/* ov016 cartridge data at 0x02113c20, 8 bytes. */
#include "types.h"

#pragma define_section data_ov016_02113c20 ".data.hasira_mid"                                      \
                                           ".data.hasira_mid"                                      \
                                           ".data.hasira_mid" far_data
#pragma section data_ov016_02113c20 begin
u32 data_ov016_02113c20[2] = {
    0xff06044c,
    0xd55600ff,
};
#pragma section data_ov016_02113c20 end
