/* claimed-storage */
/* arm9 .data 0x020991e0..0x02099224. 6 storage addresses, one object. */
#include "types.h"

extern void _ZN10dBgCh_ActrD1Ev(void);
extern void _ZN10dBgCh_ActrD0Ev(void);
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS10dBgCh_Actr[];
extern u32 _ZTI5dBgCh[];
extern u32 _ZTVN3abi17__class_type_infoE[];
extern u32 _ZTS5dBgPc[];

#pragma explicit_zero_data on
u32 _ZTI10dBgCh_Actr[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS10dBgCh_Actr,
    (u32)&_ZTI5dBgCh,
    0x42643031, 0x5f684367, 0x72746341, 0x00000000, 0x00000000,
    (u32)&_ZTI10dBgCh_Actr,
    (u32)&_ZN10dBgCh_ActrD1Ev,
    (u32)&_ZN10dBgCh_ActrD0Ev,
    0x67426435, 0x00006950, 0x67426435, 0x00006350,
    (u32)&_ZTVN3abi17__class_type_infoE,
    (u32)&_ZTS5dBgPc,
};
#pragma explicit_zero_data off
