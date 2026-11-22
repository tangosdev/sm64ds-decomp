/* claimed-storage */
/* ov075 .data.entry_gap2 0x0211ca4c..0x0211cac4. 7 storage addresses, one object. */
#include "types.h"

extern u32 data_ov075_0211cd04[];
extern u32 data_ov075_0211cd2c[];
extern u32 data_ov075_0211ccdc[];
extern u32 data_ov075_0211cd54[];
extern u32 data_ov075_0211cd7c[];
extern u32 data_ov075_0211cbc4[];
extern u32 data_ov075_0211ce94[];
extern u32 data_ov075_0211cec4[];

#pragma define_section claimsec ".data.entry_gap2" ".data.entry_gap2" ".data.entry_gap2" far_data
#pragma section claimsec begin
u32 data_ov075_0211ca4c[] = {
    (u32)&data_ov075_0211cd04,
    0x00000003,
    (u32)&data_ov075_0211cd2c,
    0x00000003,
    (u32)&data_ov075_0211ccdc,
    0x000000c8,
    (u32)&data_ov075_0211cd54,
    0x00000005,
    (u32)&data_ov075_0211ca4c,
    0x00000003,
    (u32)&data_ov075_0211cd7c,
    0x00000005, 0x01f800f8, 0x0000e119, 0x000b00ef, 0x0000e11a, 0x000400fa, 0xffffe118, 0x11f000f8,
    0x0000e119, 0x11dd00ef, 0x0000e11a, 0x11e400fa, 0xffffe118,
    (u32)&data_ov075_0211cbc4,
    0x00000004,
    (u32)&data_ov075_0211ce94,
    0x00000004,
    (u32)&data_ov075_0211cec4,
    0x00000004,
};
#pragma section claimsec end
