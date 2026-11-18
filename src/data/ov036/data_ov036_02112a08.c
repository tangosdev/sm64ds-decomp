/* ov036 cartridge data at 0x02112a08, 8 bytes. */
#include "types.h"

extern u32 data_ov036_02112a54[];

u32 data_ov036_02112a08[2] = {
    0x00000003,
    (u32)&data_ov036_02112a54,
};
