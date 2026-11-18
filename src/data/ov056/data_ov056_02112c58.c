/* ov056 cartridge data at 0x02112c58, 16 bytes. */
#include "types.h"

extern u32 data_ov056_02112bf8[];

u32 data_ov056_02112c58[4] = {
    0x073f073e,
    (u32)&data_ov056_02112bf8,
    0x0a280001,
    0x0000004f,
};
