/* ov056 cartridge data at 0x021115f4, 48 bytes. */
#include "types.h"

extern char data_ov056_02112c24[];

const u32 data_ov056_021115f4[12] = {
    0x00000001,
    0x00000001,
    0x0001021d,
    0x00000001,
    0x00000001,
    0x0000ffff,
    (u32)&data_ov056_02112c24,
    0x00000001,
    0x00000001,
    0x00000001,
    0x021d021d,
    0x00000001,
};
