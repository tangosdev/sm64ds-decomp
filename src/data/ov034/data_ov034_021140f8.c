/* ov034 cartridge data at 0x021140f8, 16 bytes. */
#include "types.h"

extern u32 data_ov034_02114094[];
extern u32 data_ov034_021141fc[];

u32 data_ov034_021140f8[4] = {
    (u32)&data_ov034_021141fc,
    (u32)&data_ov034_02114094,
    0x07e107e5,
    0x07e707e6,
};
