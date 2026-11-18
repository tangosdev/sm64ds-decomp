/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e888..0x0208e8a4, 28 bytes, 1 configured symbols.
 */
#include "types.h"

extern u32 _ZTI17dExtCommonModel_c[];

/* 0x0208e888: 28 bytes. */
u32 _ZTS17dExtShadowModel_c[7] = {
    0x45643731,
    0x68537478,
    0x776f6461,
    0x65646f4d,
    0x00635f6c,
    0x00000000,
    (u32)_ZTI17dExtCommonModel_c,
};
