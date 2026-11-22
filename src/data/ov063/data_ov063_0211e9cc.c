/* claimed-storage */
/* ov063 .data 0x0211e9cc..0x0211e9e8. 2 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS11daTrsTrap_c[];
extern u32 _ZTI8dActor_c[];

#pragma explicit_zero_data on
u32 _ZTI11daTrsTrap_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS11daTrsTrap_c,
    (u32)&_ZTI8dActor_c,
    0x61643131, 0x54737254, 0x5f706172, 0x00000063,
};
#pragma explicit_zero_data off
