/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e924..0x0208e94c, 40 bytes, 1 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__vmi_class_type_info 0x0209a754); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern char _ZTS14dExtAnmModel_c[];
extern u32 _ZTI17dExtSimpleModel_c[];
extern u32 _ZTI15dExtFrameCtrl_c[];
extern u32 _ZTI19dExtBlendAnmModel_c[];

/* 0x0208e924: 40 bytes. */
u32 _ZTI14dExtAnmModel_c[10] = {
    0x0209a754, (u32)_ZTS14dExtAnmModel_c,  0x00000000, 0x00000002, (u32)_ZTI17dExtSimpleModel_c,
    0x00000002, (u32)_ZTI15dExtFrameCtrl_c, 0x00005002, 0x00000000, (u32)_ZTI19dExtBlendAnmModel_c,
};
