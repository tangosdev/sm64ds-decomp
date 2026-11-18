/* ov056 cartridge data at 0x02112c4c, 12 bytes. */
#include "types.h"

extern u32 data_ov056_02112bc8[];
extern u32 data_ov056_02112d44[];

u32 data_ov056_02112c4c[3] = {
    (u32)&data_ov056_02112d44,
    (u32)&data_ov056_02112bc8,
    0x073b073d,
};
