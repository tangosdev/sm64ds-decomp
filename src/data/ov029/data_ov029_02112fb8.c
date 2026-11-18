/* ov029 cartridge data at 0x02112fb8, 24 bytes. */
#include "types.h"

extern u32 data_ov029_0211299c[];
extern u32 data_ov029_021129a0[];
extern u32 data_ov029_021129a4[];
extern u32 data_ov029_021129c0[];

#include "private/BTA_File.h"

#pragma define_section data_ov029_02112fb8 ".data.w12ea0"                                          \
                                           ".data.w12ea0"                                          \
                                           ".data.w12ea0" far_data
#pragma section data_ov029_02112fb8 begin

BTA_File data_ov029_02112fb8 = {
    0x005b,
    0,
    (fx32 *)&data_ov029_021129a0,
    (u16 *)&data_ov029_0211299c,
    (fx32 *)&data_ov029_021129c0,
    0x0001,
    0,
    (BTAMaterialRecord *)&data_ov029_021129a4,
};
#pragma section data_ov029_02112fb8 end
