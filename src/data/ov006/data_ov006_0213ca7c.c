/* claimed-storage */
/* ov006 .data.gap54 0x0213ca7c..0x0213ca9c. 2 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS15dScMgHanachan_c[];
extern u32 _ZTI11dScMgBase_c[];

#pragma define_section claimsec ".data.gap54" ".data.gap54" ".data.gap54" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI15dScMgHanachan_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS15dScMgHanachan_c,
    (u32)&_ZTI11dScMgBase_c,
    0x53643531, 0x48674d63, 0x63616e61, 0x5f6e6168, 0x00000063,
};
#pragma explicit_zero_data off
#pragma section claimsec end
