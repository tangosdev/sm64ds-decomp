/* claimed-storage */
/* ov034 .data.gap2 0x0211445c..0x02114468. 1 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS12daHanachan_c[];
extern u32 _ZTI12dEnemyBase_c[];

#pragma define_section claimsec ".data.gap2" ".data.gap2" ".data.gap2" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI12daHanachan_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS12daHanachan_c,
    (u32)&_ZTI12dEnemyBase_c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
