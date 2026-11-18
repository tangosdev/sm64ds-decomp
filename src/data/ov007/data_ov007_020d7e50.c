/* 0x020d7e50: Ov007Display::spriteBanks -- the six NCOB cell objects.
 * int[6], cartridge pointers. */
#include "types.h"

#pragma define_section sec_020d7e50 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section sec_020d7e50 begin
extern u8 data_ov007_020d87b8[];
extern u8 data_ov007_020d8914[];
extern u8 data_ov007_020d9794[];
extern u8 data_ov007_020d9ec0[];
extern u8 data_ov007_020da744[];
extern u8 data_ov007_020db0ac[];
int data_ov007_020d7e50[6] = {
    (int)&data_ov007_020d9794,
    (int)&data_ov007_020da744,
    (int)&data_ov007_020db0ac,
    (int)&data_ov007_020d9ec0,
    (int)&data_ov007_020d87b8,
    (int)&data_ov007_020d8914,
};
#pragma section sec_020d7e50 end
