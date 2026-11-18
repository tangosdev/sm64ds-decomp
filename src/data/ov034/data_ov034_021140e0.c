/* ov034 cartridge data at 0x021140e0, 12 bytes. */
#include "types.h"

extern u32 data_ov034_0211419c[];

u32 data_ov034_021140e0[3] = {
    0x00000600,
    (u32)&data_ov034_0211419c,
    0x00001805,
};
