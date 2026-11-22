/* claimed-storage */
/* ov032 .data.tdPre 0x02112ec0..0x02112edc. 5 storage addresses, one object. */
#include "types.h"

extern u32 data_ov032_02113008[];
extern u32 data_ov032_02113040[];

#pragma define_section claimsec ".data.tdPre" ".data.tdPre" ".data.tdPre" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov032_02112ec0[] = {
    0x00000c80, 0x000007dd, 0x03033334, 0x00000007,
    (u32)&data_ov032_02113008,
    0x00000008,
    (u32)&data_ov032_02113040,
};
#pragma explicit_zero_data off
#pragma section claimsec end
