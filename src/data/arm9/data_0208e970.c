/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e970..0x0208e980, 16 bytes, 1 configured symbols.
 */
#include "types.h"

extern void _ZThn80_N19dExtBlendAnmModel_cD1Ev();
extern void _ZThn80_N19dExtBlendAnmModel_cD0Ev();
extern u32 _ZTI14dExtAnmModel_c[];

/* 0x0208e970: 16 bytes. */
u32 VTable_dExtFrameCtrl_c_dExtBlendAnmModel_cThunk[4] = {
    (u32)&_ZThn80_N19dExtBlendAnmModel_cD1Ev,
    (u32)&_ZThn80_N19dExtBlendAnmModel_cD0Ev,
    0x00000000,
    (u32)_ZTI14dExtAnmModel_c,
};
