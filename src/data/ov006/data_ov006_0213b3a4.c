/* claimed-storage */
/* ov006 .data.gap50b 0x0213b3a4..0x0213b3f4. 3 storage addresses, one object. */
#include "types.h"

extern void _ZN19dMgTrmpObjAdapter_c5ClearEv(void);
extern void _ZN23dMgTrmpRingObjAdapter_c5ClearEv(void);
extern u32 data_ov006_0212e05c[];
extern u32 data_ov006_0212e054[];
extern u32 data_ov006_0212e130[];
extern u32 data_ov006_0212e094[];
extern u32 _ZTI19dMgTrmpObjAdapter_c[];
extern u32 _ZTI23dMgTrmpRingObjAdapter_c[];

#pragma define_section claimsec ".data.gap50b" ".data.gap50b" ".data.gap50b" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 sTrmpItemTextureAnim[] = {
    0x0000001e,
    (u32)&data_ov006_0212e05c,
    (u32)&data_ov006_0212e054,
    (u32)&data_ov006_0212e130,
    0x00000001,
    (u32)&data_ov006_0212e094,
    0x00000000,
    (u32)&_ZTI19dMgTrmpObjAdapter_c,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&_ZN19dMgTrmpObjAdapter_c5ClearEv,
    0x00000000,
    (u32)&_ZTI23dMgTrmpRingObjAdapter_c,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    (u32)&_ZN23dMgTrmpRingObjAdapter_c5ClearEv,
};
#pragma explicit_zero_data off
#pragma section claimsec end
