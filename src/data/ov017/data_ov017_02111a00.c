/* claimed-storage */
/* ov017 .data.ksPre 0x02111a00..0x02111a18. 4 storage addresses, one object. */
#include "types.h"

extern u32 data_ov017_02111ad4[];
extern u32 data_ov017_02111a50[];

#pragma define_section claimsec ".data.ksPre" ".data.ksPre" ".data.ksPre" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov017_02111a00[] = {
    0x00000765, 0x00000004, 0x00000005,
    (u32)&data_ov017_02111ad4,
    0x00000002,
    (u32)&data_ov017_02111a50,
};
#pragma explicit_zero_data off
#pragma section claimsec end
