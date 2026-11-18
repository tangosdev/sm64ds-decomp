/* ov053 cartridge data at 0x021123f4, 16 bytes. */
#include "types.h"

extern u32 data_ov053_02111384[];
extern u32 data_ov053_0211194c[];

u32 data_ov053_021123f4[4] = {
    0x0000005a,
    (u32)&data_ov053_0211194c,
    0x00000001,
    (u32)&data_ov053_02111384,
};
