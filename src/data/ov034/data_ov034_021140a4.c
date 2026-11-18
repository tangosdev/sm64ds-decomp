/* ov034 cartridge data at 0x021140a4, 12 bytes. */
#include "types.h"

extern u32 data_ov034_0211408c[];

u32 data_ov034_021140a4[3] = {
    (u32)&data_ov034_0211408c,
    0x00000000,
    0x0d480101,
};
