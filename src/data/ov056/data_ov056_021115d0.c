/* ov056 cartridge data at 0x021115d0, 16 bytes. */
#include "types.h"

extern char data_ov056_02112be0[];

const u32 data_ov056_021115d0[4] = {
    0x0000ffff,
    (u32)&data_ov056_02112be0,
    0x00000001,
    0x00000001,
};
