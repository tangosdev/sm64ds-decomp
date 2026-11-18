/* ov053 cartridge data at 0x02112404, 24 bytes. */
#include "types.h"

extern u32 data_ov053_021111a4[];
extern u32 data_ov053_021111d8[];
extern u32 data_ov053_02111314[];
extern u32 data_ov053_02111b68[];

u32 data_ov053_02112404[6] = {
    0x0000004b, (u32)&data_ov053_021111d8, (u32)&data_ov053_021111a4, (u32)&data_ov053_02111b68,
    0x00000002, (u32)&data_ov053_02111314,
};
