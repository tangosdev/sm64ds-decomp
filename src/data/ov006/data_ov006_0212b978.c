/* Word table, 14 entries. */
#include "types.h"

extern u32 data_ov006_0213add8[];
extern u32 data_ov006_0213ade8[];

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
u32 data_ov006_0212b978[14] = {
    0x0000ffff,
    (u32)&data_ov006_0213ade8,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000025,
    0x00250025,
    0x0000ffff,
    (u32)&data_ov006_0213add8,
    0x00000001,
    0x00000001,
    0x00000001,
    0x004a0025,
    0x006f0025,
};
#pragma section obj end
