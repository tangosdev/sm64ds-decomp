/* ov013 cartridge data at 0x021117dc, 8 bytes. */
#include "types.h"

extern u32 data_ov013_021117b4[];

u32 data_ov013_021117dc[2] = {
    0x00000001,
    (u32)&data_ov013_021117b4,
};
