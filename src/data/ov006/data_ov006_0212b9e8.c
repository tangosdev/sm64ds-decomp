/* Word table, 14 entries. */
#include "types.h"

extern u32 data_ov006_0213add8[];
extern u32 data_ov006_0213ade8[];

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
u32 data_ov006_0212b9e8[14] = {
    0x0000ffff,
    (u32)&data_ov006_0213ade8,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0000000c,
    0x000c000c,
    0x0000ffff,
    (u32)&data_ov006_0213add8,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0018000c,
    0x000c000c,
};
#pragma section obj end
