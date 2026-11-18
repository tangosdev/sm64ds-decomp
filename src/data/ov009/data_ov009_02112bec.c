/* ov009 cartridge data at 0x02112bec, 12 bytes. */
#include "types.h"

extern u32 data_ov009_02112b88[];

u32 data_ov009_02112bec[3] = {
    (u32)&data_ov009_02112b88,
    0x06400001,
    0x0000001f,
};
