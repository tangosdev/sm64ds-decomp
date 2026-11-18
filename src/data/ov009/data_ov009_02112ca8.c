/* ov009 cartridge data at 0x02112ca8, 32 bytes. */
#include "types.h"

extern u32 data_ov009_02112b94[];
extern u32 data_ov009_0211301c[];
extern u32 data_ov009_02113104[];
extern u32 data_ov009_02113214[];

u32 data_ov009_02112ca8[8] = {
    (u32)&data_ov009_0211301c, 0x00001c20, (u32)&data_ov009_02113214, 0x00000325,
    (u32)&data_ov009_02112b94, 0x00001140, (u32)&data_ov009_02113104, 0x00000845,
};
