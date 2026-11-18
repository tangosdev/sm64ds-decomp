/* ov014 cartridge data at 0x021147e4, 8 bytes. */
#include "types.h"

extern u32 _ZTI10daWanwan_c[];

#pragma define_section data_ov014_021147e4 ".data.gap3"                                            \
                                           ".data.gap3"                                            \
                                           ".data.gap3" far_data
#pragma section data_ov014_021147e4 begin
u32 data_ov014_021147e4[2] = {
    0x00000000,
    (u32)&_ZTI10daWanwan_c,
};
#pragma section data_ov014_021147e4 end
