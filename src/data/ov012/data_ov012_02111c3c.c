/* claimed-storage */
/* ov012 .data.c0Mid 0x02111c3c..0x02111c90. 3 storage addresses, one object. */
#include "types.h"

extern u32 data_ov012_02111774[];
extern u32 data_ov012_0211176c[];
extern u32 data_ov012_021117b0[];
extern u32 data_ov012_02111794[];
extern u32 data_ov012_02111e18[];
extern u32 data_ov012_02111ba4[];
extern u32 data_ov012_02111cf0[];

#pragma define_section claimsec ".data.c0Mid" ".data.c0Mid" ".data.c0Mid" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov012_02111c3c[] = {
    0x0000005b,
    (u32)&data_ov012_02111774,
    (u32)&data_ov012_0211176c,
    (u32)&data_ov012_021117b0,
    0x00000001,
    (u32)&data_ov012_02111794,
    (u32)&data_ov012_02111e18,
    (u32)&data_ov012_02111ba4,
    0x06fd0703, 0x07050704,
    (u32)&data_ov012_02111cf0,
    0x06400005, 0x0000000f, 0xff100000, 0x0010ffff, 0xffffff16, 0xff1a0026, 0x0040ffff, 0xffffff06,
    0xff060046, 0x0000ffff,
};
#pragma explicit_zero_data off
#pragma section claimsec end
