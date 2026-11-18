/* ov036 cartridge data at 0x02112a54, 24 bytes. */
#include "types.h"

extern u32 data_ov036_02112a44[];
extern u32 data_ov036_0211306c[];
extern u32 data_ov036_02113384[];

u32 data_ov036_02112a54[6] = {
    0x00006d00, (u32)&data_ov036_02113384, 0x00006305, (u32)&data_ov036_0211306c,
    0x00000160, (u32)&data_ov036_02112a44,
};
