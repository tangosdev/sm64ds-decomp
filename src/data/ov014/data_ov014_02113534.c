/* ov014 cartridge data at 0x02113534, 28 bytes. */
#include "types.h"

extern u32 data_ov014_02113384[];
extern u32 data_ov014_021133a8[];
extern u32 data_ov014_02113450[];
extern u32 data_ov014_02113470[];

#pragma define_section data_ov014_02113534 ".data.gap0"                                            \
                                           ".data.gap0"                                            \
                                           ".data.gap0" far_data
#pragma section data_ov014_02113534 begin
u32 data_ov014_02113534[7] = {
    (u32)&data_ov014_02113470, 0x00000407, (u32)&data_ov014_02113450, 0x00000108,
    (u32)&data_ov014_021133a8, 0x0000010e, (u32)&data_ov014_02113384,
};
#pragma section data_ov014_02113534 end
