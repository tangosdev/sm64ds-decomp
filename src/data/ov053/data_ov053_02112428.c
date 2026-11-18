/* ov053 cartridge data at 0x02112428, 12 bytes. */
#include "types.h"

extern u32 data_ov053_021111e0[];
extern u32 data_ov053_02111dc4[];

u32 data_ov053_02112428[3] = {
    (u32)&data_ov053_02111dc4,
    0x00000001,
    (u32)&data_ov053_021111e0,
};
