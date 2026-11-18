/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e8b0..0x0208e90c, 92 bytes, 4 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTI17dExtSimpleModel_c[];

/* 0x0208e8d8: 20 bytes. */
char _ZTS17dExtAnmMaterial_c[20] = "17dExtAnmMaterial_c";

/* 0x0208e8c4: 20 bytes. */
char _ZTS17dExtSimpleModel_c[20] = "17dExtSimpleModel_c";

/* 0x0208e8b0: 20 bytes. */
char _ZTS17dExtCommonModel_c[20] = "17dExtCommonModel_c";

/* 0x0208e8ec: 32 bytes. */
u32 _ZTS19dExtBlendAnmModel_c[8] = {
    0x45643931, 0x6c427478, 0x41646e65, 0x6f4d6d6e,
    0x5f6c6564, 0x00000063, 0x00000000, (u32)_ZTI17dExtSimpleModel_c,
};
