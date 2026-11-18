/* 0x020d7e50: Ov007Display::spriteBanks -- the six NCOB cell objects.
 * void *[6], cartridge pointers. */
#include "types.h"

#pragma define_section sec_020d7e50 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section sec_020d7e50 begin
extern u8 data_ov007_020d87b8[];
extern u8 data_ov007_020d8914[];
extern u8 data_ov007_020d9794[];
extern u8 data_ov007_020d9ec0[];
extern u8 data_ov007_020da744[];
extern u8 data_ov007_020db0ac[];
void *data_ov007_020d7e50[6] = {
    (void *)&data_ov007_020d9794,
    (void *)&data_ov007_020da744,
    (void *)&data_ov007_020db0ac,
    (void *)&data_ov007_020d9ec0,
    (void *)&data_ov007_020d87b8,
    (void *)&data_ov007_020d8914,
};
#pragma section sec_020d7e50 end
