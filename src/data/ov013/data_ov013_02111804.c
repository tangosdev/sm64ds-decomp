/* ov013 cartridge data at 0x02111804, 16 bytes. */
#include "types.h"

extern u32 data_ov013_02111834[];
extern u32 data_ov013_02111ae4[];

u32 data_ov013_02111804[4] = {
    0x00000b00,
    (u32)&data_ov013_02111ae4,
    0x00000205,
    (u32)&data_ov013_02111834,
};
