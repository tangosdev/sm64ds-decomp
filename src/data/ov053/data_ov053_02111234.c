/* ov053 cartridge data at 0x02111234, 16 bytes. */
#include "types.h"

extern char data_ov053_021123b0[];

const u32 data_ov053_02111234[4] = {
    0x0000ffff,
    (u32)&data_ov053_021123b0,
    0x00000001,
    0x00000001,
};
