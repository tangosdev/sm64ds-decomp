/* claimed-storage */
/* ov036 .data.gap5 0x02113b20..0x02113b2c. 1 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS19daObjRc_Kaitendai_c[];
extern u32 _ZTI16daObjKaitendai_c[];

#pragma define_section claimsec ".data.gap5" ".data.gap5" ".data.gap5" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI19daObjRc_Kaitendai_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS19daObjRc_Kaitendai_c,
    (u32)&_ZTI16daObjKaitendai_c,
};
#pragma explicit_zero_data off
#pragma section claimsec end
