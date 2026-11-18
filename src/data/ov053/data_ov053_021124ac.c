/* ov053 cartridge data at 0x021124ac, 24 bytes. */
#include "types.h"

extern u32 data_ov053_021111ac[];
extern u32 data_ov053_021111c4[];
extern u32 data_ov053_0211134c[];
extern u32 data_ov053_02111404[];

u32 data_ov053_021124ac[6] = {
    0x0000001e, (u32)&data_ov053_021111c4, (u32)&data_ov053_021111ac, (u32)&data_ov053_02111404,
    0x00000002, (u32)&data_ov053_0211134c,
};
