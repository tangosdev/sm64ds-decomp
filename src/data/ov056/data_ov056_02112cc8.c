/* ov056 cartridge data at 0x02112cc8, 8 bytes. */
#include "types.h"

extern u32 data_ov056_02112c88[];

u32 data_ov056_02112cc8[2] = {
    (u32)&data_ov056_02112c88,
    0x0000010b,
};
