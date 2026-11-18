/* ov016 cartridge data at 0x02113d4c, 64 bytes. */
#include "types.h"

extern u32 data_ov016_02113ae0[];
extern u32 data_ov016_02113ae4[];
extern u32 data_ov016_02113b00[];
extern u32 data_ov016_02113b18[];
extern u32 data_ov016_02113b28[];
extern u32 data_ov016_02113b68[];
extern u32 data_ov016_02113d0c[];
extern u32 data_ov016_02113e54[];

#pragma define_section data_ov016_02113d4c ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02113d4c begin
u32 data_ov016_02113d4c[16] = {
    0x00001602, (u32)&data_ov016_02113e54, 0x00000203, (u32)&data_ov016_02113b00,
    0x00000104, (u32)&data_ov016_02113b18, 0x0000010b, (u32)&data_ov016_02113ae0,
    0x00000401, (u32)&data_ov016_02113d0c, 0x00000208, (u32)&data_ov016_02113b68,
    0x0000010e, (u32)&data_ov016_02113ae4, 0x0000010a, (u32)&data_ov016_02113b28,
};
#pragma section data_ov016_02113d4c end
