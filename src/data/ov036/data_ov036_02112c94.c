/* ov036 cartridge data at 0x02112c94, 20 bytes. */
#include "types.h"

extern u32 data_ov036_02112c28[];
extern u32 data_ov036_02112dfc[];

u32 data_ov036_02112c94[5] = {
    0x00006802, (u32)&data_ov036_02112dfc, 0x00000703, (u32)&data_ov036_02112c28, 0x00000504,
};
