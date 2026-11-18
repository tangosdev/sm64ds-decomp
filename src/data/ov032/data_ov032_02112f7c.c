/* ov032 cartridge data at 0x02112f7c, 28 bytes. */
#include "types.h"

extern u32 data_ov032_02112ed4[];
extern u32 data_ov032_02112ee8[];
extern u32 data_ov032_0211326c[];

#pragma define_section data_ov032_02112f7c ".data.tdMid"                                           \
                                           ".data.tdMid"                                           \
                                           ".data.tdMid" far_data
#pragma section data_ov032_02112f7c begin
u32 data_ov032_02112f7c[7] = {
    (u32)&data_ov032_0211326c,
    (u32)&data_ov032_02112ed4,
    0x07dc07de,
    0x07e007df,
    (u32)&data_ov032_02112ee8,
    0x06400001,
    0x0000001f,
};
#pragma section data_ov032_02112f7c end
