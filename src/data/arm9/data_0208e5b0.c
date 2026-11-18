/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e5b0..0x0208e5c0, 16 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x0208e5b0: 16 bytes. */
char data_0208e5b0[16] = "file_id %04x";
