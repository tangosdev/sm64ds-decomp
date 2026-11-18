/* ov056 cartridge data at 0x02112bc8, 8 bytes. */
#include "types.h"

extern u32 data_ov056_02112cb4[];

u32 data_ov056_02112bc8[2] = {
    0x00000008,
    (u32)&data_ov056_02112cb4,
};
