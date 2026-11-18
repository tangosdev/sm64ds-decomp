/* ov013 cartridge data at 0x021118d0, 60 bytes. */
#include "types.h"

extern u32 data_ov013_021117ec[];
extern u32 data_ov013_021117f8[];
extern u32 data_ov013_02111860[];
extern u32 data_ov013_0211196c[];
extern u32 data_ov013_02111c54[];
extern u32 data_ov013_02111d28[];
extern u32 data_ov013_02111f18[];

u32 data_ov013_021118d0[15] = {
    0x00004c02, (u32)&data_ov013_02111f18, 0x00000803, (u32)&data_ov013_02111860,
    0x00000f04, (u32)&data_ov013_02111c54, 0x0000060b, (u32)&data_ov013_021117ec,
    0x00000f01, (u32)&data_ov013_02111d28, 0x00000809, (u32)&data_ov013_0211196c,
    0x0000060c, (u32)&data_ov013_021117f8, 0x0000010e,
};
