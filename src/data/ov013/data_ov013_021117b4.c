/* ov013 cartridge data at 0x021117b4, 8 bytes. */
#include "types.h"

extern u32 data_ov013_021117ac[];

u32 data_ov013_021117b4[2] = {
    0x00000105,
    (u32)&data_ov013_021117ac,
};
