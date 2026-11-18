/* ov056 cartridge data at 0x021115e4, 16 bytes. */
#include "types.h"

extern char data_ov056_02112bec[];

const u32 data_ov056_021115e4[4] = {
    0x0000021d,
    0x0000021d,
    0x0000ffff,
    (u32)&data_ov056_02112bec,
};
