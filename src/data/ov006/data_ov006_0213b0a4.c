/* claimed-storage */
/* ov006 .data.gap48 0x0213b0a4..0x0213b0d8. 3 storage addresses, one object. */
#include "types.h"

extern void _ZN16dMgJump3DMario_c6GetPosEv(void);
extern void _ZN16dMgJump3DMario_c11GetVelocityEv(void);
extern void _ZN16dMgJump3DMario_c10IsLaunchedEv(void);
extern u32 _ZTVN3abi20__si_class_type_infoE[];
extern u32 _ZTS16dMgJump3DMario_c[];
extern u32 _ZTI22dMg3DHeyhoObjAdapter_c[];

#pragma define_section claimsec ".data.gap48" ".data.gap48" ".data.gap48" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTI16dMgJump3DMario_c[] = {
    (u32)&_ZTVN3abi20__si_class_type_infoE,
    (u32)&_ZTS16dMgJump3DMario_c,
    (u32)&_ZTI22dMg3DHeyhoObjAdapter_c,
    0x4d643631, 0x6d754a67, 0x4d443370, 0x6f697261, 0x0000635f, 0x00000000,
    (u32)&_ZTI16dMgJump3DMario_c,
    (u32)&_ZN16dMgJump3DMario_c6GetPosEv,
    (u32)&_ZN16dMgJump3DMario_c11GetVelocityEv,
    (u32)&_ZN16dMgJump3DMario_c10IsLaunchedEv,
};
#pragma explicit_zero_data off
#pragma section claimsec end
