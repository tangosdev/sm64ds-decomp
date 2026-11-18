/* Word table, 7 entries. */
#include "types.h"

extern u32 data_ov006_0213b314[];

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
u32 data_ov006_0212e094[7] = {
    0x0000ffff,
    (u32)&data_ov006_0213b314,
    0x00000001,
    0x00000001,
    0x00000001,
    0x0000001e,
    0x00000001,
};
#pragma section obj end
