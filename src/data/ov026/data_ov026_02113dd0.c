/* claimed-storage */
/* ov026 .data.wlLate 0x02113dd0..0x02113de0. 2 storage addresses, one object. */
#include "types.h"

extern void _ZN17daWater_Suikomi_c6State0Ev(void);
extern void _ZN17daWater_Suikomi_c10InitState0Ev(void);

#pragma define_section claimsec ".data.wlLate" ".data.wlLate" ".data.wlLate" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 data_ov026_02113dd0[] = {
    (u32)&_ZN17daWater_Suikomi_c6State0Ev,
    0x00000000,
    (u32)&_ZN17daWater_Suikomi_c10InitState0Ev,
    0x00000000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
