/* ov034 cartridge data at 0x02114108, 12 bytes. */
#include "types.h"

extern u32 data_ov034_021140a4[];

u32 data_ov034_02114108[3] = {
    (u32)&data_ov034_021140a4,
    0x01f40101,
    0x0000000f,
};
