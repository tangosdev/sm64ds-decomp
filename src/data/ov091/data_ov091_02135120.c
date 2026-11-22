/* claimed-storage */
/* ov091 .data.w135120 0x02135120..0x02135138. 2 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 data_ov009_021137f8[];

#pragma define_section claimsec ".data.w135120" ".data.w135120" ".data.w135120" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTS7daDsn_c[] = {
    0x44616437, 0x635f6e73, 0x00000000,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS7daDsn_c,
    (u32)&data_ov009_021137f8,
};
#pragma explicit_zero_data off
#pragma section claimsec end
