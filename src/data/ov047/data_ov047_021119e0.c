/* claimed-storage */
/* ov047 .data_pre 0x021119e0..0x02111a74. 11 storage addresses, one object. */
#include "types.h"

extern u32 data_ov047_02111c4c[];
extern u32 data_ov047_02111a30[];
extern u32 data_ov047_021119e8[];
extern u32 data_ov047_02111ecc[];
extern u32 data_ov047_02111d54[];
extern u32 data_ov047_02111cd4[];
extern u32 data_ov047_02111a14[];

#pragma define_section claimsec ".data_pre" ".data_pre" ".data_pre" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov047_021119e0[] = {
    0x00000007,
    (u32)&data_ov047_02111c4c,
    0x00000002,
    (u32)&data_ov047_02111a30,
    0x078c078b, 0x078e078d, 0x0790078f, 0x07d007d0, 0x07d007d0, 0x096007d0, 0xff020000, 0x00027fff,
    0x7f01ff02,
    (u32)&data_ov047_021119e8,
    0x00000000, 0x19600000, 0x19920157, 0xc000e963, 0x00280000, 0x00001104, 0x00003800,
    (u32)&data_ov047_02111ecc,
    0x00002f05,
    (u32)&data_ov047_02111d54,
    0x08de0aa7, 0xf84cfc8d, 0xfc8d08de, 0x1b58060e, 0x060ee8f4, 0xea521b58,
    (u32)&data_ov047_02111cd4,
    (u32)&data_ov047_021119e0,
    0x078a0792, 0x07940793,
    (u32)&data_ov047_02111a14,
    0x06400001, 0x000000bf,
};
#pragma explicit_zero_data off
#pragma section claimsec end
