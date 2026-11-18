/* ov034 cartridge data at 0x021140ec, 12 bytes. */
#include "types.h"

extern u32 data_ov034_021140d0[];
extern u32 data_ov034_0211427c[];

u32 data_ov034_021140ec[3] = {
    (u32)&data_ov034_0211427c,
    0x000001a0,
    (u32)&data_ov034_021140d0,
};
