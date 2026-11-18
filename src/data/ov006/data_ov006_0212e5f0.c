/* Word table, 15 entries. */
#include "types.h"

extern u32 data_ov006_0213c6bc[];

#pragma define_section obj ".rodata.gap2" ".rodata.gap2" ".rodata.gap2" far_data
#pragma section obj begin
u32 data_ov006_0212e5f0[15] = {
    0x0000ffff,
    (u32)&data_ov006_0213c6bc,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00000001,
    0x00010001,
    0x00010001,
    0x00010001,
    0x00010001,
    0x00010001,
    0x00010001,
    0x00020101,
};
#pragma section obj end
