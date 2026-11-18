/* ov053 cartridge data at 0x021124c4, 16 bytes. */
#include "types.h"

extern u32 data_ov053_021111a0[];
extern u32 data_ov053_021111c8[];
extern u32 data_ov053_021113c0[];

u32 data_ov053_021124c4[4] = {
    0x00000008,
    (u32)&data_ov053_021111c8,
    (u32)&data_ov053_021111a0,
    (u32)&data_ov053_021113c0,
};
