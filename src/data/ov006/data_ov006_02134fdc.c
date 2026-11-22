/* claimed-storage */
/* ov006 .data.gap18 0x02134fdc..0x021350d8. 16 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02134ff8[];
extern u32 data_ov006_02134fe8[];
extern u32 data_ov006_02135008[];
extern u32 data_ov006_02135034[];
extern u32 data_ov006_02135024[];
extern u32 data_ov006_02135044[];
extern u32 data_ov006_0213505c[];
extern u32 data_ov006_0213507c[];
extern u32 data_ov006_021350b8[];
extern u32 data_ov006_021350a8[];
extern u32 data_ov006_021350c8[];

#pragma define_section claimsec ".data.gap18" ".data.gap18" ".data.gap18" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_02134fdc[] = {
    (u32)&data_ov006_02134ff8,
    (u32)&data_ov006_02134fe8,
    (u32)&data_ov006_02135008,
    0x41f800f7, 0x000011d2, 0x81f000f0, 0xffff2102, 0x41f800f7, 0x000011d2, 0x81f000f0, 0xffff0102,
    0x41f800f7, 0x000011d2, 0x81f000f0, 0xffff1102,
    (u32)&data_ov006_02135034,
    (u32)&data_ov006_02135024,
    (u32)&data_ov006_02135044,
    0x41f800f8, 0x000010c2, 0x81f040f8, 0xffff2122, 0x41f800f8, 0x000010c2, 0x81f040f8, 0xffff0122,
    0x41f800f8, 0x000010c2, 0x81f040f8, 0xffff1122,
    (u32)&data_ov006_0213505c,
    (u32)&data_ov006_0213507c,
    0x81c040f8, 0x00000080, 0x81e040f8, 0x00000084, 0x800040f8, 0x00000088, 0x802040f8, 0xffff008c,
    0x81c040f8, 0x000000c0, 0x81e040f8, 0x000000c4, 0x800040f8, 0x000000c8, 0x802040f8, 0xffff00cc,
    (u32)&data_ov006_021350b8,
    (u32)&data_ov006_021350a8,
    (u32)&data_ov006_021350c8,
    0x41f800f8, 0x000010c4, 0x81f040f8, 0xffff2122, 0x41f800f8, 0x000010c4, 0x81f040f8, 0xffff0122,
    0x41f800f8, 0x000010c4, 0x81f040f8, 0xffff1122,
};
#pragma explicit_zero_data off
#pragma section claimsec end
