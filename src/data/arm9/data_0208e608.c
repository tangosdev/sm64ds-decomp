/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e608..0x0208e624, 28 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x0208e608: 8 bytes. */
char data_0208e608[8] = "%s:%u";

/* 0x0208e610: 20 bytes. */
char data_0208e610[20] = "RootHeap%4d+%4dKB";
