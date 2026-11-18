/* ov056 cartridge data at 0x02112bf8, 12 bytes. */
#include "types.h"

extern u32 data_ov056_02112bd0[];
extern u32 data_ov056_02112c34[];

u32 data_ov056_02112bf8[3] = {
    (u32)&data_ov056_02112bd0,
    (u32)&data_ov056_02112c34,
    0x27100000,
};
