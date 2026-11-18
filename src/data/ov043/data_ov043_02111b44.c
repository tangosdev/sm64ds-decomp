/* ov043 cartridge data at 0x02111b44, 28 bytes. */
#include "types.h"

extern u32 data_ov043_02111b08[];
extern u32 data_ov043_02111b18[];
extern u32 data_ov043_02111d60[];

#pragma define_section data_ov043_02111b44 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov043_02111b44 begin
u32 data_ov043_02111b44[7] = {
    (u32)&data_ov043_02111d60,
    (u32)&data_ov043_02111b08,
    0x076f0772,
    0x07740773,
    (u32)&data_ov043_02111b18,
    0x06400001,
    0x000000af,
};
#pragma section data_ov043_02111b44 end
