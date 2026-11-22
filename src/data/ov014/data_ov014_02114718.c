/* claimed-storage */
/* ov014 .data.gap1 0x02114718..0x02114724. 1 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS10daWanwan_c[];
extern u32 _ZTI12dEnemyBase_c[];

#pragma define_section claimsec ".data.gap1" ".data.gap1" ".data.gap1" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI10daWanwan_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS10daWanwan_c,
    (u32)&_ZTI12dEnemyBase_c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
