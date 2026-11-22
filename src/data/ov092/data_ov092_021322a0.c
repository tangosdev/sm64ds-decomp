/* claimed-storage */
/* ov092 .data.gap_rtti 0x021322a0..0x021322c8. 2 storage addresses, one object. */
#include "types.h"

extern void _Z18daOnms_c_classInitv(void);
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS8daOnms_c[];
extern u32 _ZTI10dBgActor_c[];

#pragma define_section claimsec ".data.gap_rtti" ".data.gap_rtti" ".data.gap_rtti" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI8daOnms_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS8daOnms_c,
    (u32)&_ZTI10dBgActor_c,
    (u32)&_Z18daOnms_c_classInitv,
    0x01320135, 0x02000002, 0x000fa000, 0x002ee000, 0x01fa0000, 0x01fa0000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
