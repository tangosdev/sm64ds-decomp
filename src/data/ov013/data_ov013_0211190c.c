/* ov013 cartridge data at 0x0211190c, 12 bytes. */
#include "types.h"

extern u32 data_ov013_02111780[];
extern u32 data_ov013_02111a3c[];

u32 data_ov013_0211190c[3] = {
    (u32)&data_ov013_02111780,
    0x00000c0a,
    (u32)&data_ov013_02111a3c,
};
