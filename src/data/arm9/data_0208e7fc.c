/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e7fc..0x0208e868, 108 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u32 _ZTI17dExtShadowModel_c[];

/* 0x0208e838: 20 bytes. */
char _ZTS15dExtAnm2Model_c[20] = "15dExtAnm2Model_c";

/* 0x0208e824: 20 bytes. */
char _ZTS15dExtAnmTexPat_c[20] = "15dExtAnmTexPat_c";

/* 0x0208e810: 20 bytes. */
char _ZTS15dExtAnmTexSRT_c[20] = "15dExtAnmTexSRT_c";

/* 0x0208e7fc: 20 bytes. */
char _ZTS14dExtAnmModel_c[20] = "14dExtAnmModel_c";

/* 0x0208e84c: 28 bytes. */
u32 _ZTS15dExtFrameCtrl_c[7] = {
    0x45643531,
    0x72467478,
    0x43656d61,
    0x5f6c7274,
    0x00000063,
    0x00000000,
    (u32)_ZTI17dExtShadowModel_c,
};
