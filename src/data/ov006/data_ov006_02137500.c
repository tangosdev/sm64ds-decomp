/* claimed-storage */
/* ov006 .data.gap28 0x02137500..0x02137590. 13 storage addresses, one object. */
#include "types.h"

extern u32 data_ov006_02137558[];
extern u32 data_ov006_02137550[];
extern u32 data_ov006_02137548[];
extern u32 data_ov006_02137540[];
extern u32 data_ov006_02137538[];
extern u32 data_ov006_02137530[];
extern u32 data_ov006_02137528[];
extern u32 data_ov006_02137520[];
extern u32 data_ov006_02137518[];
extern u32 data_ov006_02137510[];
extern u32 data_ov006_02137508[];

#pragma define_section claimsec ".data.gap28" ".data.gap28" ".data.gap28" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov006_02137500[] = {
    0x81f000f0, 0xffff5090, 0x81f000f0, 0xffff4090, 0x81f000f0, 0xffff0090, 0x81f000f0, 0xffff5010,
    0x81f000f0, 0xffff4010, 0x81f000f0, 0xffff0010, 0x81f000f0, 0xffff508c, 0x81f000f0, 0xffff408c,
    0x81f000f0, 0xffff008c, 0x81f000f0, 0xffff500c, 0x81f000f0, 0xffff400c, 0x81f000f0, 0xffff000c,
    (u32)&data_ov006_02137558,
    (u32)&data_ov006_02137550,
    (u32)&data_ov006_02137548,
    (u32)&data_ov006_02137540,
    (u32)&data_ov006_02137538,
    (u32)&data_ov006_02137530,
    (u32)&data_ov006_02137528,
    (u32)&data_ov006_02137520,
    (u32)&data_ov006_02137518,
    (u32)&data_ov006_02137510,
    (u32)&data_ov006_02137508,
    (u32)&data_ov006_02137500,
};
#pragma explicit_zero_data off
#pragma section claimsec end
