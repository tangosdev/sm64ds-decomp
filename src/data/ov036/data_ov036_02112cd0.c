/* ov036 cartridge data at 0x02112cd0, 4 bytes. */
#include "types.h"

extern u32 data_ov036_02112a04[];

u32 data_ov036_02112cd0[1] = {
    (u32)&data_ov036_02112a04,
};
