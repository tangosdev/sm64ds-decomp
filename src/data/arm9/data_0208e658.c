/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e658..0x0208e660, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0208e658: 8 bytes. */
char _ZTS5dCc_c[8] = "5dCc_c";
