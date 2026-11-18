/* ov013 cartridge data at 0x02111784, 8 bytes. */
#include "types.h"

extern u32 data_ov013_02111804[];

u32 data_ov013_02111784[2] = {
    0x00000002,
    (u32)&data_ov013_02111804,
};
