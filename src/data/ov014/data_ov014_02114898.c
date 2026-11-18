/* ov014 cartridge data at 0x02114898, 24 bytes. */
#include "types.h"

extern u32 _ZTI20daObjWanwanShutter_c[];

#pragma define_section data_ov014_02114898 ".data.gap3"                                            \
                                           ".data.gap3"                                            \
                                           ".data.gap3" far_data
#pragma section data_ov014_02114898 begin
u32 data_ov014_02114898[6] = {
    0x00100000, 0x00800000, 0x02000000, 0x00000000, 0x00000000, (u32)&_ZTI20daObjWanwanShutter_c,
};
#pragma section data_ov014_02114898 end
