/* ov053 cartridge data at 0x021124d4, 8 bytes. */
#include "types.h"

extern u32 data_ov053_021112dc[];

u32 data_ov053_021124d4[2] = {
    0x00000002,
    (u32)&data_ov053_021112dc,
};
