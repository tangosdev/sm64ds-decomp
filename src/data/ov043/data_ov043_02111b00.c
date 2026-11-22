/* claimed-storage */
/* ov043 .data.gap0 0x02111b00..0x02111b60. 9 storage addresses, one object. */
#include "types.h"

extern u32 data_ov043_02111ca0[];
extern u32 data_ov043_02111b34[];
extern u32 data_ov043_02111b10[];
extern u32 data_ov043_02111fd0[];
extern u32 data_ov043_02111dc0[];
extern u32 data_ov043_02111d60[];
extern u32 data_ov043_02111b08[];
extern u32 data_ov043_02111b18[];

#pragma define_section claimsec ".data.gap0" ".data.gap0" ".data.gap0" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov043_02111b00[] = {
    0x000006a4, 0x07710770, 0x00000005,
    (u32)&data_ov043_02111ca0,
    0x00000002,
    (u32)&data_ov043_02111b34,
    (u32)&data_ov043_02111b10,
    0x00000000, 0x0af00000, 0x0b2219ff, 0xc000005d, 0x00240000, 0x00001104, 0x00002a00,
    (u32)&data_ov043_02111fd0,
    0x00004205,
    (u32)&data_ov043_02111dc0,
    (u32)&data_ov043_02111d60,
    (u32)&data_ov043_02111b08,
    0x076f0772, 0x07740773,
    (u32)&data_ov043_02111b18,
    0x06400001, 0x000000af,
};
#pragma explicit_zero_data off
#pragma section claimsec end
