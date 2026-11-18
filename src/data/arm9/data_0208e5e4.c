/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e5e4..0x0208e5fc, 24 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x0208e5e4: 12 bytes. */
char data_0208e5e4[12] = "%.3s:%08X";

/* 0x0208e5f0: 12 bytes. */
char data_0208e5f0[12] = "SPSR%08X";
