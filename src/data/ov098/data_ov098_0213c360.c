/* claimed-storage */
/* ov098 .data.blockHead 0x0213c360..0x0213c380. 2 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS15daObjYajirusi_c[];
extern u32 _ZTI10dBgActor_c[];

#pragma define_section claimsec ".data.blockHead" ".data.blockHead" ".data.blockHead" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI15daObjYajirusi_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS15daObjYajirusi_c,
    (u32)&_ZTI10dBgActor_c,
    0x61643531, 0x596a624f, 0x72696a61, 0x5f697375, 0x00000063,
};
#pragma explicit_zero_data off
#pragma section claimsec end
