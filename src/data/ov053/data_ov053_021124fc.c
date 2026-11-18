/* ov053 cartridge data at 0x021124fc, 24 bytes. */
#include "types.h"

extern u32 data_ov053_02112384[];
extern u32 data_ov053_021123c8[];

u32 data_ov053_021124fc[6] = {
    (u32)&data_ov053_02112384, 0x072c072e, 0x0730072f,
    (u32)&data_ov053_021123c8, 0x03200101, 0x0000009f,
};
