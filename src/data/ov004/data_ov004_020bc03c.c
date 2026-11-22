/* claimed-storage */
/* ov004 .data.gap24 0x020bc03c..0x020bc070. 2 storage addresses, one object. */
#include "types.h"

extern void _ZN11dScMgBase_c15graphCallback_c14GraphCallback0Ev(void);
extern void _ZN11dScMgBase_c15graphCallback_c14GraphCallback1Ev(void);
extern void _ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev(void);
extern void _ZN11dScMgBase_c15graphCallback_c14GraphCallback3Ev(void);

#pragma define_section claimsec ".data.gap24" ".data.gap24" ".data.gap24" far_data
#pragma section claimsec begin
#pragma explicit_zero_data on
u32 _ZTVN11dScMgBase_c15graphCallback_cE[] = {
    (u32)&_ZN11dScMgBase_c15graphCallback_c14GraphCallback0Ev,
    (u32)&_ZN11dScMgBase_c15graphCallback_c14GraphCallback1Ev,
    (u32)&_ZN11dScMgBase_c15graphCallback_c14GraphCallback2Ev,
    (u32)&_ZN11dScMgBase_c15graphCallback_c14GraphCallback3Ev,
    0x6431314e, 0x674d6353, 0x65736142, 0x3531635f, 0x70617267, 0x6c614368, 0x6361626c, 0x45635f6b,
    0x00000000,
};
#pragma explicit_zero_data off
#pragma section claimsec end
