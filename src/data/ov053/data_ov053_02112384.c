/* ov053 cartridge data at 0x02112384, 8 bytes. */
#include "types.h"

extern u32 data_ov053_0211247c[];

u32 data_ov053_02112384[2] = {
    0x00000003,
    (u32)&data_ov053_0211247c,
};
