/* ov053 cartridge data at 0x021123c8, 12 bytes. */
#include "types.h"

extern u32 data_ov053_0211239c[];
extern u32 data_ov053_0211241c[];

u32 data_ov053_021123c8[3] = {
    (u32)&data_ov053_0211239c,
    (u32)&data_ov053_0211241c,
    0x06400000,
};
