/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e624..0x0208e648, 36 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x0208e624: 16 bytes. */
char data_0208e624[16] = "RootHeap Broken";

/* 0x0208e634: 20 bytes. */
char data_0208e634[20] = "GameHeap%4d+%4dKB";
