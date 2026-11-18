/* ov056 cartridge data at 0x02112bd0, 8 bytes. */
#include "types.h"

extern u32 data_ov056_02112c14[];

u32 data_ov056_02112bd0[2] = {
    0x00000002,
    (u32)&data_ov056_02112c14,
};
