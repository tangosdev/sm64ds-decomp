/* ov014 cartridge data at 0x02113554, 124 bytes. */
#include "types.h"

extern u32 data_ov014_02113388[];
extern u32 data_ov014_02113390[];
extern u32 data_ov014_021133bc[];
extern u32 data_ov014_021133cc[];
extern u32 data_ov014_021133dc[];
extern u32 data_ov014_02113404[];
extern u32 data_ov014_0211341c[];
extern u32 data_ov014_021135d0[];
extern u32 data_ov014_02113670[];
extern u32 data_ov014_02113710[];
extern u32 data_ov014_021137b0[];
extern u32 data_ov014_02113860[];
extern u32 data_ov014_02113910[];
extern u32 data_ov014_02113bd8[];
extern u32 data_ov014_02113d08[];
extern u32 data_ov014_02113fc8[];

#pragma define_section data_ov014_02113554 ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section data_ov014_02113554 begin
u32 data_ov014_02113554[31] = {
    (u32)&data_ov014_02113fc8, 0x00005805, (u32)&data_ov014_02113d08, 0x00000c20, (u32)&data_ov014_02113910, 0x00000125,
    (u32)&data_ov014_02113388, 0x00000b40, (u32)&data_ov014_021137b0, 0x00000145, (u32)&data_ov014_02113390, 0x00000a60,
    (u32)&data_ov014_021135d0, 0x00000365, (u32)&data_ov014_02113404, 0x00001380, (u32)&data_ov014_02113bd8, 0x00000385,
    (u32)&data_ov014_0211341c, 0x00000aa0, (u32)&data_ov014_02113670, 0x000002a5, (u32)&data_ov014_021133cc, 0x00000bc0,
    (u32)&data_ov014_02113860, 0x000002c5, (u32)&data_ov014_021133dc, 0x00000ae0, (u32)&data_ov014_02113710, 0x000002e5,
    (u32)&data_ov014_021133bc,
};
#pragma section data_ov014_02113554 end
