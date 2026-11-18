/* ov043 cartridge data at 0x02111ca0, 40 bytes. */
#include "types.h"

extern u32 data_ov043_02111b00[];
extern u32 data_ov043_02111b04[];
extern u32 data_ov043_02111b24[];
extern u32 data_ov043_02111cc8[];
extern u32 data_ov043_02111d10[];

u32 data_ov043_02111ca0[10] = {
    0x00000504, (u32)&data_ov043_02111cc8, 0x0000020b, (u32)&data_ov043_02111b04,
    0x00000501, (u32)&data_ov043_02111d10, 0x0000010c, (u32)&data_ov043_02111b00,
    0x0000010a, (u32)&data_ov043_02111b24,
};
