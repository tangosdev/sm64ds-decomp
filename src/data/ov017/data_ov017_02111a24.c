/* claimed-storage */
/* ov017 .data.w11a24 0x02111a24..0x02111a60. 6 storage addresses, one object. */
#include "types.h"

extern u32 data_ov017_02111a10[];
extern u32 data_ov017_02111afc[];
extern u32 data_ov017_02111b6c[];

#pragma define_section claimsec ".data.w11a24" ".data.w11a24" ".data.w11a24" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov017_02111a24[] = {
    (u32)&data_ov017_02111a10,
    0x00000000, 0x0c2b0000, 0x0384ff02, 0xfc1803b6, 0x80000000, 0x00000000, 0xba875701, 0xb3b02af8,
    0xc0700001, 0x94700000, 0x00000300,
    (u32)&data_ov017_02111afc,
    0x00000905,
    (u32)&data_ov017_02111b6c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
