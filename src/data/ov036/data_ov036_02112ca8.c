/* ov036 cartridge data at 0x02112ca8, 32 bytes. */
#include "types.h"

extern u32 data_ov036_02112a00[];
extern u32 data_ov036_02112a24[];
extern u32 data_ov036_02112c54[];
extern u32 data_ov036_02112cd4[];

u32 data_ov036_02112ca8[8] = {
    (u32)&data_ov036_02112cd4, 0x0000010b, (u32)&data_ov036_02112a00, 0x00000401,
    (u32)&data_ov036_02112c54, 0x00000206, (u32)&data_ov036_02112a24, 0x00000207,
};
