/* ov016 cartridge data at 0x02114498, 180 bytes. */
#include "types.h"

#pragma define_section data_ov016_02114498 ".data.hasira"                                          \
                                           ".data.hasira"                                          \
                                           ".data.hasira" far_data
#pragma section data_ov016_02114498 begin
u32 data_ov016_02114498[45] = {
    0xea52004c, 0x0190044c, 0xaaaaffff, 0x044c0000, 0xe700004c, 0x0af0044c, 0x8e38ffff, 0x07480000,
    0xe5d4001e, 0x0c1c0546, 0x00000000, 0xff060000, 0x003400f3, 0x0a640947, 0x00000000, 0x011f0000,
    0x028300f3, 0x0ca20a0f, 0x00000000, 0x011f0000, 0x042600f3, 0x0e7c087f, 0x00000000, 0x011f0000,
    0xf65500f3, 0x1752041a, 0x00000000, 0x015e0000, 0xf7fe0107, 0x0e42fc18, 0xe0000000, 0xffff0000,
    0xf9c00107, 0x128efc18, 0xa0000000, 0xffff0000, 0x02e40107, 0x1040fe0c, 0xd5560000, 0xffff0000,
    0xefd70032, 0x082604c4, 0x00000000, 0x00024000, 0x1450004c,
};
#pragma section data_ov016_02114498 end
