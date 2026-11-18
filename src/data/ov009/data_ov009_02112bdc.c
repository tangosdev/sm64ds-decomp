/* ov009 cartridge data at 0x02112bdc, 16 bytes. */
#include "types.h"

extern u32 data_ov009_02112b74[];
extern u32 data_ov009_02112e7c[];

u32 data_ov009_02112bdc[4] = {
    (u32)&data_ov009_02112e7c,
    (u32)&data_ov009_02112b74,
    0x07950797,
    0x07990798,
};
