/* ov009 cartridge data at 0x02112b88, 12 bytes. */
#include "types.h"

extern u32 data_ov009_02112b6c[];
extern u32 data_ov009_02112bac[];

u32 data_ov009_02112b88[3] = {
    (u32)&data_ov009_02112b6c,
    (u32)&data_ov009_02112bac,
    0x3a980000,
};
