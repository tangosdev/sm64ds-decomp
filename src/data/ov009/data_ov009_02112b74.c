/* ov009 cartridge data at 0x02112b74, 8 bytes. */
#include "types.h"

extern u32 data_ov009_02112dcc[];

u32 data_ov009_02112b74[2] = {
    0x00000009,
    (u32)&data_ov009_02112dcc,
};
