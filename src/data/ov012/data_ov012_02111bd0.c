/* claimed-storage */
/* ov012 .data.w11bd0 0x02111bd0..0x02111c24. 9 storage addresses, one object. */
#include "types.h"

extern u32 data_ov012_02111cb0[];
extern u32 data_ov012_02111bf4[];

#pragma define_section claimsec ".data.w11bd0" ".data.w11bd0" ".data.w11bd0" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov012_02111bd0[] = {
    0x08980898, 0x08980898, 0x00000898, 0x06ff06fe, 0x07010700, 0x00000702, 0x5f74616d, 0x65746177,
    0x00000072, 0x0772443f, 0xf7a2faec, 0x0777e43c, 0xf501fb70, 0x00000200,
    (u32)&data_ov012_02111cb0,
    0x00000205,
    (u32)&data_ov012_02111bf4,
    0x0d16002a, 0x079efbe6, 0xc0000000, 0x27770000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
