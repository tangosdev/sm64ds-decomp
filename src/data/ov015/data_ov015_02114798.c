/* claimed-storage */
/* ov015 .data_gap4b 0x02114798..0x021147a4. 1 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS17daObjBk_Ukisima_c[];
extern u32 _ZTI16daObjKaitendai_c[];

#pragma define_section claimsec ".data_gap4b" ".data_gap4b" ".data_gap4b" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI17daObjBk_Ukisima_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS17daObjBk_Ukisima_c,
    (u32)&_ZTI16daObjKaitendai_c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
