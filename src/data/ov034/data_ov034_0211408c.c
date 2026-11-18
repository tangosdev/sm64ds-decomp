/* ov034 cartridge data at 0x0211408c, 8 bytes. */
#include "types.h"

extern u32 data_ov034_021140e0[];

u32 data_ov034_0211408c[2] = {
    0x00000003,
    (u32)&data_ov034_021140e0,
};
