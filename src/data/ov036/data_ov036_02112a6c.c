/* ov036 cartridge data at 0x02112a6c, 28 bytes. */
#include "types.h"

extern u32 data_ov036_02112a10[];
extern u32 data_ov036_02112a18[];
extern u32 data_ov036_02112d1c[];

u32 data_ov036_02112a6c[7] = {
    (u32)&data_ov036_02112d1c,
    (u32)&data_ov036_02112a10,
    0x07a907ab,
    0x07ad07ac,
    (u32)&data_ov036_02112a18,
    0x06400101,
    0x0000002f,
};
