/* ov053 cartridge data at 0x02112494, 24 bytes. */
#include "types.h"

extern u32 data_ov053_021111b0[];
extern u32 data_ov053_021111dc[];
extern u32 data_ov053_021112a4[];
extern u32 data_ov053_02111750[];

u32 data_ov053_02112494[6] = {
    0x0000003f, (u32)&data_ov053_021111dc, (u32)&data_ov053_021111b0, (u32)&data_ov053_02111750,
    0x00000002, (u32)&data_ov053_021112a4,
};
