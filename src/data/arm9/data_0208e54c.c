/* Crash-screen text and the dCc RTTI records.
 * The printf format strings ShowCrashScreen prints (RSP, R0-R12, PC, RA,
 * CPSR registers), then the _ZTS/_ZTI records of the dCc/dCcAc/dCcPos/
 * dCcAcPos/dClipper classes and the dExt* model family.
 * ROM 0x0208e54c..0x0208e58c, 64 bytes, 4 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x0208e56c: 16 bytes. */
char data_0208e56c[16] = "LayerNo  %1d";

/* 0x0208e55c: 16 bytes. */
char data_0208e55c[16] = "PlayerID %1d";

/* 0x0208e54c: 16 bytes. */
char data_0208e54c[16] = "StageNo %2d:%s";

/* 0x0208e57c: 16 bytes. */
char data_0208e57c[16] = "RoomNo   %1d";
