/* claimed-storage */
/* ov065 .data 0x0211cd34..0x0211cd68. 4 storage addresses, one object. */
#include "types.h"

extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTI8dActor_c[];
extern u32 _ZTS12daDossyCap_c[];

#pragma explicit_zero_data on
u32 _ZTS9daDossy_c[] = {
    0x44616439, 0x7973736f, 0x0000635f,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS9daDossy_c,
    (u32)&_ZTI8dActor_c,
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS12daDossyCap_c,
    (u32)&_ZTI8dActor_c,
    0x61643231, 0x73736f44, 0x70614379, 0x0000635f,
};
#pragma explicit_zero_data off
