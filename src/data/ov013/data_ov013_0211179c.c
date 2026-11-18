/* ov013 cartridge data at 0x0211179c, 8 bytes. */
#include "types.h"

extern u32 data_ov013_021117c4[];

u32 data_ov013_0211179c[2] = {
    0x00000105,
    (u32)&data_ov013_021117c4,
};
