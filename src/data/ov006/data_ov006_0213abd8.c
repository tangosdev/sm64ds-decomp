/* claimed-storage */
/* ov006 .data.gap43 0x0213abd8..0x0213ac48. 10 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_0213abec[];
extern u32 data_ov006_0213ac04[];
extern u32 data_ov006_0213ac28[];
extern u32 data_ov006_0213ac34[];
extern u32 data_ov006_0213ac40[];

#pragma define_section claimsec ".data.gap43" ".data.gap43" ".data.gap43" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_0213abd8[] = {
    0xc1e000e0, 0xffff0000,
    (u32)&data_ov006_0213abd8,
    (u32)&data_ov006_0213abec,
    (u32)&data_ov006_0213ac04,
    0x81f080e8, 0x00000022, 0x41f00008, 0x000000a2, 0x01f800e0, 0xffff0003, 0x800080e8, 0x00000024,
    0x40000008, 0x000000a4, 0x000000e0, 0x00000004, 0x001080f8, 0xffff0066,
    (u32)&data_ov006_0213ac28,
    0xc1e88000, 0xffff211c,
    (u32)&data_ov006_0213ac34,
    0xc1e000e0, 0xffff5018,
    (u32)&data_ov006_0213ac40,
    0x81f000f0, 0xffff5074,
};
#pragma explicit_zero_data off
#pragma section claimsec end
