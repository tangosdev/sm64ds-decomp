/* claimed-storage */
/* ov029 .data.gap0 0x02112ea0..0x02112fb8. 22 storage addresses, one object. */
#include "types.h"

extern u32 data_ov029_02112f88[];
extern u32 data_ov029_021131b4[];
extern u32 data_ov029_02113174[];
extern u32 data_ov029_021134ec[];
extern u32 data_ov029_021133d4[];
extern u32 data_ov029_02112ed8[];
extern u32 data_ov029_02112eb0[];
extern u32 data_ov029_02112ea8[];

#pragma define_section claimsec ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov029_02112ea0[] = {
    0x00013004, 0x07ef07ee, 0x00000003,
    (u32)&data_ov029_02112f88,
    0x0000000a,
    (u32)&data_ov029_021131b4,
    0x00000008,
    (u32)&data_ov029_02113174,
    0x5f74616d, 0x65746177, 0x00000072, 0xf9080190, 0xc000f358, 0x00001099, 0xfc7c0014, 0x0258fe0c,
    0x0aaa0003, 0xffff0000, 0xffffff00, 0xea60c350, 0xf7976c01, 0xafc80000, 0x0cb2f510, 0x10000b86,
    0x0000032c, 0x00000dea, 0x11300017, 0x03e80000, 0x00000000, 0xffff0000, 0x11300017, 0x03e80000,
    0x00000000, 0xffff0000, 0x11300017, 0x03e80000, 0x00000000, 0xffff0000, 0x11300017, 0x03e80000,
    0x00000000, 0xffff0000, 0x11300017, 0x03e80000, 0x00000000, 0xffff0000, 0x11300017, 0x03e80000,
    0x00003e93, 0xffff0000, 0x11300017, 0x03e80000, 0x00000000, 0xffff0000, 0x0cb2f510, 0x00000b86,
    0x0000032c, 0x00200dea, 0x00001e00,
    (u32)&data_ov029_021134ec,
    0x00002305,
    (u32)&data_ov029_021133d4,
    0x000001a0,
    (u32)&data_ov029_02112ed8,
    (u32)&data_ov029_02112eb0,
    0x00000000, 0x109a0001,
    (u32)&data_ov029_02112ea8,
    0x00000000, 0x00000001,
};
#pragma explicit_zero_data off
#pragma section claimsec end
